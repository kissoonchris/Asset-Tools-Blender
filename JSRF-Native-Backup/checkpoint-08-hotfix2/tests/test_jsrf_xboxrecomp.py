import struct
import tempfile
import unittest
from pathlib import Path

from scripts.jsrf_xboxrecomp import (
    JSRF_ENTRY_POINT,
    JSRF_SHA256,
    JSRF_TITLE_ID,
    XBOXRECOMP_REVISION,
    _python_argv,
    build_pipeline_commands,
    parse_xbe_identity,
    validate_jsrf_xbe,
)


def make_synthetic_xbe(path: Path) -> None:
    data = bytearray(0x1200)
    data[0:4] = b"XBEH"
    base = 0x00010000
    cert_va = base + 0x200
    sec_va = base + 0x400
    entry = JSRF_ENTRY_POINT
    thunk = 0x001C3F60
    struct.pack_into("<I", data, 0x104, base)
    struct.pack_into("<I", data, 0x108, 0x1000)
    struct.pack_into("<I", data, 0x10C, 0x280000)
    struct.pack_into("<I", data, 0x118, cert_va)
    struct.pack_into("<I", data, 0x11C, 1)
    struct.pack_into("<I", data, 0x120, sec_va)
    struct.pack_into("<I", data, 0x128, entry ^ 0xA8FC57AB)
    struct.pack_into("<I", data, 0x158, thunk ^ 0x5B6D40B6)

    cert_off = cert_va - base
    struct.pack_into("<I", data, cert_off + 0, 0x1D0)
    struct.pack_into("<I", data, cert_off + 8, JSRF_TITLE_ID)
    title = "Jet Set Radio Future".encode("utf-16le")
    data[cert_off + 12: cert_off + 12 + len(title)] = title

    sec_off = sec_va - base
    name_va = base + 0x500
    struct.pack_into("<IIIIII", data, sec_off, 0x16, 0x11000, 0x1000, 0x1000, 0x1000, name_va)
    data[0x500:0x506] = b".text\0"
    path.write_bytes(data)


class XbeIdentityTests(unittest.TestCase):
    def test_parse_decodes_retail_entry_title_and_sections(self):
        with tempfile.TemporaryDirectory() as td:
            xbe = Path(td) / "default.xbe"
            make_synthetic_xbe(xbe)
            info = parse_xbe_identity(xbe)
            self.assertEqual(info["entry_point"], JSRF_ENTRY_POINT)
            self.assertEqual(info["kernel_thunk"], 0x001C3F60)
            self.assertEqual(info["title_id"], JSRF_TITLE_ID)
            self.assertEqual(info["title_name"], "Jet Set Radio Future")
            self.assertEqual(info["sections"][0]["name"], ".text")
            self.assertEqual(info["sections"][0]["virtual_address"], 0x11000)

    def test_validate_rejects_non_matching_hash_when_required(self):
        with tempfile.TemporaryDirectory() as td:
            xbe = Path(td) / "default.xbe"
            make_synthetic_xbe(xbe)
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                validate_jsrf_xbe(xbe, require_hash=True)

    def test_validate_accepts_synthetic_identity_without_hash_gate(self):
        with tempfile.TemporaryDirectory() as td:
            xbe = Path(td) / "default.xbe"
            make_synthetic_xbe(xbe)
            info = validate_jsrf_xbe(xbe, require_hash=False)
            self.assertEqual(info["title_id"], JSRF_TITLE_ID)
            self.assertEqual(info["entry_point"], JSRF_ENTRY_POINT)


class PipelineCommandTests(unittest.TestCase):
    def test_windows_python_command_strips_quotes_from_executable(self):
        import scripts.jsrf_xboxrecomp as driver
        original_name = driver.os.name
        try:
            driver.os.name = "nt"
            self.assertEqual(
                _python_argv('"C:\\Program Files\\Python\\python.exe" -3'),
                ["C:\\Program Files\\Python\\python.exe", "-3"],
            )
        finally:
            driver.os.name = original_name

    def test_pipeline_uses_official_order_and_gen_dir(self):
        commands = build_pipeline_commands(
            Path("C:/tools/xboxrecomp"),
            Path("C:/games/JSRF"),
            Path("C:/work/jsrf"),
            "py -3",
        )
        self.assertEqual(len(commands), 4)
        self.assertIn("tools.xbe_parser", commands[0])
        self.assertIn("tools.disasm", commands[1])
        self.assertIn("tools.func_id", commands[2])
        self.assertIn("tools.recomp", commands[3])
        self.assertIn("--all", commands[3])
        self.assertIn("--split", commands[3])
        gen_idx = commands[3].index("--gen-dir") + 1
        self.assertTrue(commands[3][gen_idx].replace("\\", "/").endswith("/recomp/src/recomp/gen"))


class RecompProjectStructureTests(unittest.TestCase):
    def test_jsrf_host_project_has_retail_boot_invariants(self):
        root = Path(__file__).resolve().parents[1]
        cmake = (root / "recomp" / "CMakeLists.txt").read_text(encoding="utf-8")
        main_c = (root / "recomp" / "src" / "main.c").read_text(encoding="utf-8")
        manual_c = (root / "recomp" / "src" / "recomp_manual.c").read_text(encoding="utf-8")

        self.assertIn("project(jsrf_recomp C)", cmake)
        self.assertIn("add_executable(${PROJECT_NAME}\n", cmake)
        self.assertNotIn("add_executable(${PROJECT_NAME} WIN32", cmake)
        self.assertIn("src/recomp/gen/*.c", cmake)
        self.assertIn("target_link_libraries(${PROJECT_NAME} PRIVATE xboxrecomp)", cmake)
        self.assertIn("dbghelp", cmake)

        self.assertIn("#define JSRF_ENTRY_POINT 0x00148023", main_c)
        self.assertIn('getenv("JSRF_GAME_DIR")', main_c)
        self.assertIn("recomp_dispatch_init()", main_c)
        self.assertIn("xbox_WatchdogStart()", main_c)
        self.assertIn("xbe_entry_point();", main_c)

        self.assertIn("recomp_lookup_manual", manual_c)
        self.assertIn("return (recomp_func_t)0;", manual_c)
        self.assertNotIn("if (xbox_va ==", manual_c)

class WindowsBootstrapStructureTests(unittest.TestCase):
    def test_windows_bootstrap_pins_toolkit_and_builds_generated_host(self):
        root = Path(__file__).resolve().parents[1]
        ps1 = (root / "scripts" / "Build-JSRF-XboxRecomp.ps1").read_text(encoding="utf-8")
        bat = (root / "Build-JSRF-XboxRecomp.bat").read_text(encoding="utf-8")

        self.assertIn(XBOXRECOMP_REVISION, ps1)
        self.assertIn("sp00nznet/xboxrecomp.git", ps1)
        self.assertIn("-m pip install", ps1)
        self.assertIn("capstone", ps1)
        self.assertIn("jsrf_xboxrecomp.py", ps1)
        self.assertIn('"generate"', ps1)
        self.assertIn("cmake", ps1.lower())
        self.assertIn("--config", ps1)
        self.assertIn("Release", ps1)
        self.assertIn("JSRF_GAME_DIR", ps1)
        self.assertIn("jsrf_recomp.exe", ps1)
        self.assertIn("Launching native JSRF runtime", ps1)
        self.assertIn("Native runtime exited with code", ps1)
        self.assertIn("Build-JSRF-XboxRecomp.ps1", bat)
        self.assertIn("-Run", bat)


if __name__ == "__main__":
    unittest.main()

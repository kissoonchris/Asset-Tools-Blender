from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]

INVALID_COLON = re.compile(r'\$(?!env:|global:|script:|local:|private:|using:)([A-Za-z_][A-Za-z0-9_]*):(?=[\s"\'])')


def test_no_ambiguous_variable_colon_in_powershell_scripts():
    failures = []
    for path in sorted((ROOT / "scripts").glob("*.ps1")):
        for lineno, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            match = INVALID_COLON.search(line)
            if match:
                failures.append(f"{path.relative_to(ROOT)}:{lineno}: ${match.group(1)}:")
    assert not failures, "Ambiguous PowerShell variable followed by colon: " + "; ".join(failures)

# JSRF v44 implementation backup

This records the production change applied by `scripts/jsrf_xboxrecomp.py` in v44. The final binary/source ZIPs and exact unified patch are also backed up in ChatGPT Library under `/JSRF/`.

## Driver patch

```python
def patch_xboxrecomp_directory_query_context_lifetime(toolkit_dir: str | Path) -> None:
    """Retire NtQueryDirectoryFile search state when its directory is closed.

    The Windows file backend keys FindFirst/FindNext state by the native HANDLE
    returned for the directory. bridge_NtClose historically bypassed
    xbox_NtClose and called CloseHandle directly, while kernel_file.c kept the
    DIR_CONTEXT alive. Windows may recycle that HANDLE value for the next
    directory open. A subsequent NtQueryDirectoryFile with RestartScan=0 can
    then inherit the old search and report a file from the prior query.

    JSRF's retail FindFirstFileA wrapper uses exactly that open/query/close
    sequence for JSRF_CACHE_COMPLETEXX.CMP probes.
    """
    toolkit_dir = Path(toolkit_dir)
    bridge_path = toolkit_dir / "src" / "kernel" / "kernel_bridge.c"
    file_path = toolkit_dir / "src" / "kernel" / "kernel_file.c"
    for path in (bridge_path, file_path):
        if not path.is_file():
            raise FileNotFoundError(f"xboxrecomp file-I/O source was not found: {path}")

    bridge = bridge_path.read_text(encoding="utf-8")
    file_text = file_path.read_text(encoding="utf-8")

    marker = "release_dir_context_for_file(Handle);"
    bridge_fixed = "xbox_NtClose(h);"
    trace_marker = "[DIRQUERY]"
    if marker in file_text and bridge_fixed in bridge and trace_marker in bridge:
        return
    if marker in file_text or bridge_fixed in bridge or trace_marker in bridge:
        raise RuntimeError("xboxrecomp directory-query lifetime patch is only partially installed")

    bridge_close_old = (
        "        HANDLE h = bridge_take_handle(raw_handle);\n"
        "        if (h && h != INVALID_HANDLE_VALUE)\n"
        "            CloseHandle(h);\n"
    )
    bridge_close_new = (
        "        HANDLE h = bridge_take_handle(raw_handle);\n"
        "        if (h && h != INVALID_HANDLE_VALUE)\n"
        "            xbox_NtClose(h);\n"
    )
    if bridge_close_old not in bridge:
        raise RuntimeError("Pinned xboxrecomp NtClose bridge no longer matches expected source")
    bridge = bridge.replace(bridge_close_old, bridge_close_new, 1)

    close_signature = "NTSTATUS __stdcall xbox_NtClose(HANDLE Handle)\n"
    if close_signature not in file_text:
        raise RuntimeError("Pinned xboxrecomp file close function no longer matches expected source")
    file_text = file_text.replace(
        close_signature,
        "static void release_dir_context_for_file(HANDLE FileHandle);\n\n" + close_signature,
        1,
    )

    close_body_old = (
        "    if (Handle && Handle != INVALID_HANDLE_VALUE) {\n"
        "        CloseHandle(Handle);\n"
        "        return STATUS_SUCCESS;\n"
        "    }\n"
    )
    close_body_new = (
        "    if (Handle && Handle != INVALID_HANDLE_VALUE) {\n"
        "        release_dir_context_for_file(Handle);\n"
        "        CloseHandle(Handle);\n"
        "        return STATUS_SUCCESS;\n"
        "    }\n"
    )
    if close_body_old not in file_text:
        raise RuntimeError("Pinned xboxrecomp Win32 NtClose body no longer matches expected source")
    file_text = file_text.replace(close_body_old, close_body_new, 1)

    dir_globals = (
        "static DIR_CONTEXT s_dir_contexts[MAX_DIR_CONTEXTS];\n"
        "static CRITICAL_SECTION s_dir_cs;\n"
        "static BOOL s_dir_cs_init = FALSE;\n\n"
    )
    if dir_globals not in file_text:
        raise RuntimeError("Pinned xboxrecomp Win32 directory context globals no longer match expected source")
    release_helper = (
        "static void release_dir_context_for_file(HANDLE FileHandle)\n"
        "{\n"
        "    int i;\n\n"
        "    if (!FileHandle || !s_dir_cs_init)\n"
        "        return;\n\n"
        "    EnterCriticalSection(&s_dir_cs);\n"
        "    for (i = 0; i < MAX_DIR_CONTEXTS; i++) {\n"
        "        DIR_CONTEXT *ctx = &s_dir_contexts[i];\n"
        "        if (ctx->file_handle != FileHandle)\n"
        "            continue;\n"
        "        if (ctx->find_handle && ctx->find_handle != INVALID_HANDLE_VALUE)\n"
        "            FindClose(ctx->find_handle);\n"
        "        ctx->file_handle = NULL;\n"
        "        ctx->find_handle = NULL;\n"
        "        ctx->first_done = FALSE;\n"
        "        memset(&ctx->find_data, 0, sizeof(ctx->find_data));\n"
        "    }\n"
        "    LeaveCriticalSection(&s_dir_cs);\n"
        "}\n\n"
    )
    file_text = file_text.replace(dir_globals, dir_globals + release_helper, 1)

    query_tail = (
        "    g_eax = (uint32_t)xbox_NtQueryDirectoryFile(handle, NULL, NULL, NULL, &ios,\n"
        "                XBOX_TO_NATIVE(info_va), length, pfn, (BOOLEAN)restart);\n"
        "    bridge_write_iostatus(ios_va, ios.Status, (uint32_t)ios.Information);\n"
    )
    if query_tail not in bridge:
        raise RuntimeError("Pinned xboxrecomp NtQueryDirectoryFile bridge no longer matches expected source")
    query_trace = (
        "    g_eax = (uint32_t)xbox_NtQueryDirectoryFile(handle, NULL, NULL, NULL, &ios,\n"
        "                XBOX_TO_NATIVE(info_va), length, pfn, (BOOLEAN)restart);\n"
        "    if (pfn && fn.Buffer) {\n"
        "        static const char jsrf_marker[] = \"JSRF_CACHE_COMPLETE\";\n"
        "        uint32_t i;\n"
        "        for (i = 0; i + sizeof(jsrf_marker) - 1 <= fn.Length; i++) {\n"
        "            if (memcmp(fn.Buffer + i, jsrf_marker, sizeof(jsrf_marker) - 1) == 0) {\n"
        "                fprintf(stderr,\n"
        "                        \"  [DIRQUERY] handle=%p pattern='%.*s' restart=%u \"\n"
        "                        \"status=0x%08X ios=0x%08X info=%u\\n\",\n"
        "                        handle, (int)fn.Length, fn.Buffer, restart,\n"
        "                        g_eax, (uint32_t)ios.Status, (uint32_t)ios.Information);\n"
        "                fflush(stderr);\n"
        "                break;\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    bridge_write_iostatus(ios_va, ios.Status, (uint32_t)ios.Information);\n"
    )
    bridge = bridge.replace(query_tail, query_trace, 1)

    bridge_path.write_text(bridge, encoding="utf-8")
    file_path.write_text(file_text, encoding="utf-8")
```

`run_pipeline()` calls `patch_xboxrecomp_directory_query_context_lifetime(toolkit_dir)` immediately after the existing kernel-dispatch TLS patch and before xboxrecomp code generation.

## Checksums

- `JSRF-Native-Easy-Test-Checkpoint-08-FIXED-v44.zip`: `2c87b0319c23dbd883ee24a5c1ed12b035af904fe152a45f11c1a28237784021`
- `JSRF-Native-Source-Checkpoint-08-HOTFIX43.zip`: `2c87b0319c23dbd883ee24a5c1ed12b035af904fe152a45f11c1a28237784021`
- `JSRF-V43-to-V44.patch`: `8ce369242bb7ccc9c0748203003e98c7943e38cfde10b5ccc49a823e94316afc`

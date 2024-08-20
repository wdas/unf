import pathlib
import re
import subprocess
import shutil

ROOT = pathlib.Path().resolve()


def build():
    """Build API documentation."""
    target = "unfApiRefDoc"

    build_path = ROOT / "build"
    output_path = ROOT / "api"

    # Run CMake commands
    subprocess.call(["cmake", "-S", ROOT, "-B", str(build_path)])
    subprocess.call(["cmake", "--build", str(build_path), "--target", target])

    # Remove output_path if it exists
    if output_path.exists() and output_path.is_dir():
        shutil.rmtree(output_path)

    # Move doxygen API outside the build directory
    output_path.mkdir()

    shutil.move(build_path / "doxygen", output_path / "doxygen")
    shutil.move(build_path / "UNF.tag", output_path / "UNF.tag")

    return output_path


def create_cmake_config():
    """Create cmake config to build api docs."""
    content = "\n".join([
        fetch_project_content(),
        fetch_api_doc_content()
    ])

    path = ROOT / "CMakeLists.txt"
    path.write_text(content)

    return path


def fetch_project_content():
    """Fetch info from main cmake config."""
    # cmake version available on RTD via apt install is lower than 3.20,
    # so we hard-code it for now.
    content = "cmake_minimum_required(VERSION 3.15)\n\n"

    path = ROOT.parent.parent / "CMakeLists.txt"
    data = path.read_text()

    patterns = [
        r"project\(.*?\)",
        r"find_package\(\s*Doxygen.*?\)",
    ]

    for pattern in patterns:
        match = re.search(pattern, data, re.MULTILINE | re.DOTALL)
        if not match:
            raise ValueError(f"Pattern not found: {pattern!r}")

        content += f"{match.group()}\n\n"

    return content


def fetch_api_doc_content():
    """Fetch info from documentation cmake config."""
    content = ""

    path = ROOT.parent / "CMakeLists.txt"
    data = path.read_text()

    # Extract DOXYGEN-related settings
    options = re.findall(r"set\(\s*DOXYGEN.*?\s*\)", data, re.MULTILINE | re.DOTALL)
    content += "\n".join(options) + "\n"

    # Update path to tag file.
    content = re.sub("/doc/", "/../", content)

    # Update doxygen html output path.
    content = re.sub(
        r"(set\(\s*DOXYGEN_HTML_OUTPUT )(.*?)(\s*\))",
        r'\g<1>"doxygen"\g<3>',
        content, re.MULTILINE | re.DOTALL
    )

    # Update tag file path.
    content = re.sub(
        r"(set\(\s*DOXYGEN_GENERATE_TAGFILE )(.*?)(\s*\))",
        r'\g<1>"${CMAKE_CURRENT_BINARY_DIR}/UNF.tag"\g<3>',
        content, re.MULTILINE | re.DOTALL
    )

    match = re.search(
        r"doxygen_add_docs\(.*?\)", data,
        re.MULTILINE | re.DOTALL
    )
    if not match:
        raise ValueError("No command found to build doxygen docs")

    # Update doxygen input paths.
    command = re.sub("/doc/", "/../", match.group())
    command = re.sub("/src/", "/../../src/", command)

    content += f"\n{command}"
    return content

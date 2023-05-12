# -*- coding: utf-8 -*-

import os
import re
import subprocess
import shutil

ROOT = os.path.normpath(os.path.abspath(os.path.curdir))


def build():
    """Build API documentation."""
    target = "unfApiRefDoc"

    build_path = os.path.join(ROOT, "build")
    subprocess.call(["cmake", "-S", ROOT, "-B", build_path])
    subprocess.call(["cmake", "--build", build_path, "--target", target])

    # Move doxygen API outside the build directory.
    output_path = os.path.join(ROOT, "api")
    os.makedirs(output_path)

    shutil.move(
        os.path.join(build_path, "doxygen"),
        os.path.join(output_path, "doxygen")
    )

    shutil.move(
        os.path.join(build_path, "UNF.tag"),
        os.path.join(output_path, "UNF.tag")
    )

    return output_path


def create_cmake_config():
    """Create cmake config to build api docs."""
    content = "\n".join([
        fetch_project_content(),
        fetch_api_doc_content()
    ])

    path = os.path.join(ROOT, "CMakeLists.txt")

    with open(path, "w") as stream:
        stream.write(content)

    return path


def fetch_project_content():
    """Fetch info from main cmake config."""
    # cmake version available on RTD via apt install is lower than 3.20,
    # so we hard-code it for now.
    content = "cmake_minimum_required(VERSION 3.15)\n\n"

    path = os.path.join(ROOT, "..", "..", "CMakeLists.txt")

    with open(path, "r") as stream:
        data = stream.read()

    patterns = [
        r"project\(.*?\)",
        r"find_package\(\s*Doxygen.*?\)",
    ]

    for pattern in patterns:
        match = re.search(pattern, data, re.MULTILINE | re.DOTALL)
        if not match:
            raise ValueError("Pattern not found: {!r}".format(pattern))

        content += "{}\n\n".format(match.group())

    return content


def fetch_api_doc_content():
    """Fetch info from documentation cmake config."""
    content = ""

    path = os.path.join(ROOT, "..", "CMakeLists.txt")

    with open(path, "r") as stream:
        data = stream.read()

    for option in re.findall(
        r"set\(\s*DOXYGEN.*?\s*\)", data, re.MULTILINE | re.DOTALL
    ):
        content += "{}\n".format(option)

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

    content += "\n{}".format(command)
    return content

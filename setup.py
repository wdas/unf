# -*- coding: utf-8 -*-

from skbuild import setup

import pathlib

ROOT = pathlib.Path(__file__).parent.resolve()
LONG_DESCRIPTION = (ROOT / "README.md").read_text(encoding="utf-8")

# Compute dependencies.
INSTALL_REQUIRES = [
    "usd-core >= 20.11"
]

DOC_REQUIRES = [
    "sphinx >= 1.8, < 2",
    "sphinx_rtd_theme >= 0.1.6, < 1",
    "lowdown >= 0.2.0, < 2"
]

TEST_REQUIRES = [
    "pytest-runner >= 6, < 7",
    "pytest >= 7, < 8",
    "pytest-xdist >= 2, < 3",
    "pytest-cov >= 3, < 4",
]

def cmake_process_manifest_hook(cmake_manifiest):
    """Skip headers and static library from installation."""
    return [
        name for name in cmake_manifiest
        if pathlib.Path(name).suffix not in [".h", ".a"]
    ]

# Execute setup.
setup(
    name="usd-notice-framework",
    version="0.1.0",
    description="Notice management library built over USD Notices",
    long_description=LONG_DESCRIPTION,
    url="https://github.com/wdas/usd-notice-broker",
    keywords=["usd", "notice"],
    author="Walt Disney Animation Studios",
    python_requires=">=3.6, <3.10",
    install_requires=INSTALL_REQUIRES,
    tests_require=TEST_REQUIRES,
    extras_require={
        "doc": DOC_REQUIRES,
        "test": TEST_REQUIRES,
        "dev": DOC_REQUIRES + TEST_REQUIRES
    },
    zip_safe=False,
    cmake_args=[
        "-DBUILD_PYTHON_BINDINGS=True",
        "-DBUILD_TESTS=False",
        "-DBUILD_SHARED_LIBS=False",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_POSITION_INDEPENDENT_CODE=True",
        "-DBoost_USE_STATIC_LIBS=False"
    ],
    cmake_process_manifest_hook=cmake_process_manifest_hook,
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Topic :: Software Development",
    ]
)

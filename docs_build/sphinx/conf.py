# -*- coding: utf-8 -*-
import os
from pathlib import Path

# -- Project information -----------------------------------------------------
exec(open("../project_common.py").read())


rst_prolog = ".. |xml| replace:: %s\n" % (project)

extensions.append('breathe');
extensions.append('exhale');

breathe_projects = {
  "doc_c":      "_doxygen/"+"doc_c/xml-c",
  "doc_cpp":    "_doxygen/"+"doc_cpp/xml-cpp",
  "doc_json":   "_doxygen/"+"doc_json/xml-json",
  "doc_matlab": "_doxygen/"+"doc_matlab/xml-matlab",
  "doc_lua":    "_doxygen/"+"doc_lua/xml-lua",
}

breathe_default_project = "doc_cpp"

dir_path_inc = os.path.dirname(os.path.realpath(__file__))+"../../../include"
dir_path_inc = Path(dir_path_inc).resolve()

dir_path_c   = os.path.dirname(os.path.realpath(__file__))+"../../../src"
dir_path_c   = Path(dir_path_c).resolve()

dir_path_cpp = os.path.dirname(os.path.realpath(__file__))+"../../../src"
dir_path_cpp = Path(dir_path_cpp).resolve()

dir_path_json = os.path.dirname(os.path.realpath(__file__))+"../../../src_json_interface"
dir_path_json = Path(dir_path_json).resolve()

dir_path_matlab = os.path.dirname(os.path.realpath(__file__))+"../../../src_matlab_interface"
dir_path_matlab = Path(dir_path_matlab).resolve()

dir_path_lua = os.path.dirname(os.path.realpath(__file__))+"../../../src_lua_interface"
dir_path_lua = Path(dir_path_lua).resolve()

doxygen_common_stdin = """
        EXTRACT_ALL         = YES
        SOURCE_BROWSER      = YES
        EXTRACT_STATIC      = YES
        HIDE_SCOPE_NAMES    = NO
        CALLER_GRAPH        = YES
        GRAPHICAL_HIERARCHY = YES
        HAVE_DOT            = YES
        QUIET               = NO
        GENERATE_TREEVIEW   = YES
        SHORT_NAMES         = YES
        IMAGE_PATH          = ../images

        XML_PROGRAMLISTING    = YES
        RECURSIVE             = NO
        FULL_PATH_NAMES       = YES
        ENABLE_PREPROCESSING  = YES
        MACRO_EXPANSION       = YES
        SKIP_FUNCTION_MACROS  = NO
        EXPAND_ONLY_PREDEF    = NO
        INHERIT_DOCS          = YES
        INLINE_INHERITED_MEMB = YES
        EXTRACT_PRIVATE       = YES
        PREDEFINED           += protected=private
        GENERATE_HTML         = NO
"""

doc_c = {
    'verboseBuild':          True,
    "rootFileName":          "root.rst",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "doxygenStripFromPath":  str(dir_path_inc),
    "exhaleDoxygenStdin":   '''
        INPUT               = ../../include/GenericContainer
        PREDEFINED         += protected=private
        XML_OUTPUT          = xml-c
        FILE_PATTERNS       = GenericContainerCinterface.h
'''+doxygen_common_stdin,
    "containmentFolder":    os.path.realpath('./api-c'),
    "rootFileTitle":        "C API",
}

doc_cpp = {
    'verboseBuild':          True,
    "rootFileName":          "root.rst",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "doxygenStripFromPath":  str(dir_path_cpp),
    "exhaleDoxygenStdin":   '''
        INPUT               = ../../src ../../include/GenericContainer
        PREDEFINED         += protected=private
        XML_OUTPUT          = xml-cpp
        EXCLUDE_PATTERNS    = GenericContainerCinterface.* GenericContainerJson*.* GenericContainerMatlabInterface.* GenericContainerLua*.*
'''+doxygen_common_stdin,
    "containmentFolder":    os.path.realpath('./api-cpp'),
    "rootFileTitle":        "C++ API",
}

doc_json = {
    'verboseBuild':          True,
    "rootFileName":          "root.rst",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "doxygenStripFromPath":  str(dir_path_json),
    "exhaleDoxygenStdin":   '''
        INPUT               = ../../src_json_interface ../../include/GenericContainer
        PREDEFINED         += protected=private
        XML_OUTPUT          = xml-json
        FILE_PATTERNS       = GenericContainerJson*.*
'''+doxygen_common_stdin,
    "containmentFolder":    os.path.realpath('./api-json'),
    "rootFileTitle":        "JSON API",
}

doc_matlab = {
    'verboseBuild':          True,
    "rootFileName":          "root.rst",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "doxygenStripFromPath":  str(dir_path_matlab),
    "exhaleDoxygenStdin":   '''
        INPUT               = ../../src_matlab_interface ../../include/GenericContainer
        PREDEFINED         += protected=private
        XML_OUTPUT          = xml-matlab
        FILE_PATTERNS       = GenericContainerMatlabInterface.*
'''+doxygen_common_stdin,
    "containmentFolder":    os.path.realpath('./api-matlab'),
    "rootFileTitle":        "MATLAB API",
}

doc_lua = {
    'verboseBuild':          True,
    "rootFileName":          "root.rst",
    "createTreeView":        True,
    "exhaleExecutesDoxygen": True,
    "doxygenStripFromPath":  str(dir_path_lua),
    "exhaleDoxygenStdin":   '''
        INPUT               = ../../src_lua_interface ../../include/GenericContainer
        PREDEFINED         += protected=private
        XML_OUTPUT          = xml-lua
        FILE_PATTERNS       = GenericContainerLua*.hh GenericContainerLua*.cc
'''+doxygen_common_stdin,
    "containmentFolder":    os.path.realpath('./api-lua'),
    "rootFileTitle":        "LUA API",
}

exhale_projects_args = {
  # Third Party Project Includes
  "doc_c":      doc_c,
  "doc_cpp":    doc_cpp,
  "doc_json":   doc_json,
  "doc_matlab": doc_matlab,
  "doc_lua":    doc_lua
}

cpp_index_common_prefix = ['GC_namespace::']

# If false, no module index is generated.
html_domain_indices = True

# If false, no index is generated.
html_use_index = True

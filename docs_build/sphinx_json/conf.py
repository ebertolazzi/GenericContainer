# -*- coding: utf-8 -*-
import os
from past.builtins import execfile

# -- Project information -----------------------------------------------------
execfile('../project_common.py')

# Setup the breathe extension
breathe_projects = { project+"_json": "../xml-json" }
breathe_default_project = project+"_json"

extensions.append('exhale');
extensions.append('breathe');

# Setup the exhale extension
exhale_args = {
  # These arguments are required
  "containmentFolder":     "./api-json",
  "rootFileName":          "library_root.rst",
  "rootFileTitle":         "JSON API",
  "doxygenStripFromPath":  "..",
  # Suggested optional arguments
  "createTreeView":        True,
  # TIP: if using the sphinx-bootstrap-theme, you need
  "treeViewIsBootstrap": True,
  "exhaleExecutesDoxygen": True,
  #"exhaleDoxygenStdin":    "INPUT = ../../src"
  "exhaleDoxygenStdin":
'''
        EXTRACT_ALL           = YES
        SOURCE_BROWSER        = YES
        EXTRACT_STATIC        = YES
        HIDE_SCOPE_NAMES      = NO
        CALLER_GRAPH          = YES
        GRAPHICAL_HIERARCHY   = YES
        HAVE_DOT              = YES
        QUIET                 = NO
        INPUT                 = ../../src_json_interface
        GENERATE_TREEVIEW     = YES
        XML_OUTPUT            = xml-json

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
        EXCLUDE_PATTERNS      = JSON-test.cc
        GENERATE_HTML         = NO
''',
  "lexerMapping": { r".*\.m": "MATLAB" }
}

cpp_index_common_prefix = ['GC_namespace::']

html_theme_options['logotarget'] = "../index"
html_theme_options['roottarget'] = "../index"
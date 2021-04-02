# -*- coding: utf-8 -*-
import os
from past.builtins import execfile

# -- Project information -----------------------------------------------------
execfile('../project_common.py')

# Setup the breathe extension
breathe_projects = { project+"_cpp": "../xml-cpp" }
breathe_default_project = project+"_cpp"

extensions.append('exhale');
extensions.append('breathe');

# Setup the exhale extension
exhale_args = {
  # These arguments are required
  "containmentFolder":     "./api-cpp",
  "rootFileName":          "library_root.rst",
  "rootFileTitle":         "C++ API",
  "doxygenStripFromPath":  "..",
  # Suggested optional arguments
  "createTreeView":        True,
  # TIP: if using the sphinx-bootstrap-theme, you need
  "treeViewIsBootstrap": True,
  "exhaleExecutesDoxygen": True,
  #"exhaleDoxygenStdin":    "INPUT = ../../src"
  "exhaleDoxygenStdin":
'''
        EXTRACT_ALL         = YES
        SOURCE_BROWSER      = YES
        EXTRACT_STATIC      = YES
        HIDE_SCOPE_NAMES    = NO
        CALLER_GRAPH        = YES
        GRAPHICAL_HIERARCHY = YES
        HAVE_DOT            = YES
        QUIET               = NO
        INPUT               = ../../src
        GENERATE_TREEVIEW   = YES
        XML_OUTPUT          = xml-cpp

        XML_PROGRAMLISTING    = YES
        RECURSIVE             = YES
        FULL_PATH_NAMES       = YES
        ENABLE_PREPROCESSING  = YES
        MACRO_EXPANSION       = YES
        SKIP_FUNCTION_MACROS  = NO
        EXPAND_ONLY_PREDEF    = NO
        INHERIT_DOCS          = YES
        INLINE_INHERITED_MEMB = YES
        EXTRACT_PRIVATE       = YES
        PREDEFINED           += protected=private
        EXCLUDE_PATTERNS      = GenericContainerCinterface.*
        GENERATE_HTML         = NO
''',
  "lexerMapping": { r".*\.m": "MATLAB" }
}

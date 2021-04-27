# -*- coding: utf-8 -*-
import os
from pathlib import Path

# -- Project information -----------------------------------------------------
exec(open("../project_common.py").read())


# Setup the breathe extension
breathe_projects = { project+"_c": "../xml-c" }
breathe_default_project = project+"_c"

extensions.append('exhale');
extensions.append('breathe');

dir_path = os.path.dirname(os.path.realpath(__file__))+"../../../src"
dir_path = Path(dir_path).resolve()

# Setup the exhale extension
exhale_args = {
  # These arguments are required
  "containmentFolder":     "./api-c",
  "rootFileName":          "library_root.rst",
  "rootFileTitle":         "C API",
  "doxygenStripFromPath":  str(dir_path),
  # Suggested optional arguments
  "createTreeView":        True,
  # TIP: if using the sphinx-bootstrap-theme, you need
  "treeViewIsBootstrap": False,
  "exhaleExecutesDoxygen": True,
  #"exhaleDoxygenStdin":    "INPUT = ../../src"
  "exhaleDoxygenStdin":
'''
        EXTRACT_ALL         = YES
        SOURCE_BROWSER      = YES
        EXTRACT_STATIC      = YES
        HIDE_SCOPE_NAMES    = NO
        CALLER_GRAPH        = NO
        GRAPHICAL_HIERARCHY = NO
        HAVE_DOT            = YES
        QUIET               = NO
        INPUT               = ../../src
        GENERATE_TREEVIEW   = YES
        XML_OUTPUT          = xml-c
        OPTIMIZE_OUTPUT_FOR_C = YES
        SHORT_NAMES           = YES

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
        FILE_PATTERNS         = ../../src/GenericContainerCinterface.* *.h *.c
        GENERATE_HTML         = NO
''',
  "lexerMapping": { r".*\.m": "MATLAB" }
}

html_theme_options['logotarget'] = "../index"
html_theme_options['roottarget'] = "../index"
TEMPLATE = subdirs

SUBDIRS = \
    src/runtime  \
    src/gui_app

src/gui_app.depends = src/runtime
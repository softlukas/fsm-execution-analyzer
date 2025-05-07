# 0_proj_qt/0_proj_qt.pro

TEMPLATE = subdirs # Hlavný projekt riadi podprojekty

# Zoznam podprojektov (adresárov obsahujúcich .pro súbory)
SUBDIRS = \
    src/runtime  \
    src/gui_app

# Nastavenie závislosti: gui_app závisí od runtime
# To zabezpečí, že runtime knižnica sa skompiluje skôr, ako sa začne linkovať gui_app
src/gui_app.depends = src/runtime

# Voliteľné: Ak chceš, aby 'make clean' v tomto adresári vyčistil aj podprojekty
# CONFIG += ordered # Môže pomôcť s poradím

# Tu už nedefinuješ SOURCES, HEADERS, atď. pre celý projekt

QMAKE_CXXFLAGS += -fsanitize=address -g
QMAKE_LFLAGS += -fsanitize=address
# Makefile - Umiestnený v 0_proj_qt/

# --- Premenné ---
QMAKE    ?= qmake
MAKE     ?= make # Explicitne definuj make
MKDIR = mkdir -p
RM = rm -rf


# Adresáre
BUILD_DIR    = build # Build adresár pre VŠETKO (qmake si vytvorí podadresáre)
SRC_DIR      = src

ARCHIVE_NAME = submission
DOC_DIR = doc
TEMPLATES_DIR = templates
THIRDPARTY_DIR = third_party

# Cesta k hlavnému .pro súboru
MAIN_PRO_FILE = 0_proj_qt.pro

# Názov finálneho GUI targetu (bude v build/gui_app/)
GUI_EXEC_NAME = IfaAutomatonTool
GUI_TARGET = $(BUILD_DIR)/gui_app/$(GUI_EXEC_NAME) # Cesta k GUI v podprojekte

# --- Hlavné Ciele ---

# Predvolený cieľ: Zostaví celý projekt (runtime knižnicu a potom GUI)
all: clean $(GUI_TARGET) # Závisí od finálneho produktu

# Pravidlo pre zostavenie všetkého
# Spustí qmake na hlavný .pro a potom make
$(GUI_TARGET): $(MAIN_PRO_FILE) $(SRC_DIR)/**/*.* FORCE
	@echo ">>> Building project using main SUBDIRS project..."
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR) && $(QMAKE) $(CURDIR)/$(MAIN_PRO_FILE) -o Makefile # Generuj hlavný Makefile v build
	$(MAKE) -C $(BUILD_DIR)  # Spusti make v build (ten zavolá make pre subdirs)
	@echo "<<< Project built successfully. GUI Tool: $(GUI_TARGET)"

# Cieľ pre vyčistenie
clean:
	@echo ">>> Cleaning build directory..."
	$(RM) $(BUILD_DIR)
	@echo "<<< Clean finished."

# Cieľ pre Doxygen (zostáva rovnaký)
doxygen:
	# ... (príkazy pre doxygen) ...

# Cieľ pre Pack (zostáva podobný, kopíruješ aj nové .pro súbory)
pack: clean
	@echo ">>> Creating submission archive: $(ARCHIVE_NAME)..."
	$(RM) $(ARCHIVE_NAME).tar $(ARCHIVE_NAME).tar.gz $(ARCHIVE_NAME).zip
	$(MKDIR) $(ARCHIVE_NAME)
	# Skopíruj potrebné adresáre a súbory
	cp -r $(SRC_DIR) $(DOC_DIR) $(TEMPLATES_DIR) $(THIRDPARTY_DIR) Makefile README.txt $(MAIN_PRO_FILE) $(SRC_DIR)/runtime/runtime.pro $(SRC_DIR)/gui_app/gui_app.pro $(ARCHIVE_NAME)/
	# cp -r $(EXAMPLES_DIR) $(ARCHIVE_NAME)/ # Ak máš examples
	$(RM) $(ARCHIVE_NAME)/$(DOC_DIR)/html
	# Vytvor archív
	$(TAR) -czf $(ARCHIVE_NAME).tar.gz -C $(ARCHIVE_NAME) . --owner=0 --group=0
	# $(ZIP) -r -q $(ARCHIVE_NAME).zip $(ARCHIVE_NAME)/
	$(RM) $(ARCHIVE_NAME)
	@echo "<<< Archive created: $(ARCHIVE_NAME).tar.gz"
	@echo "!!! Nezabudni skontrolovať obsah archívu a či ide skompilovať (`make clean && make`) !!!"

# Pomocný cieľ
FORCE:

# Phony targets
.PHONY: all gui clean doxygen pack FORCE
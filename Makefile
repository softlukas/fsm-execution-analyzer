# Makefile - Umiestnený v 0_proj_qt/

# --- Premenné ---
QMAKE    ?= qmake
MAKE     ?= make # Explicitne definuj make
MKDIR = mkdir -p
RM = rm -rf
TAR = tar
ZIP = zip

# Adresáre
BUILD_DIR    = build
SRC_DIR      = src

ARCHIVE_NAME = submission
DOC_DIR = doc
TEMPLATES_DIR = templates
THIRDPARTY_DIR = third_party
EXAMPLES_DIR = examples
# Cesta k hlavnému .pro súboru
MAIN_PRO_FILE = main_pro.pro

# Názov finálneho GUI targetu (bude v build/gui_app/src/)
GUI_EXEC_NAME = AutomationCreator
GUI_TARGET = $(BUILD_DIR)/src/gui_app/$(GUI_EXEC_NAME) # Cesta k GUI v podprojekte

# --- Hlavné Ciele ---


all: clean $(GUI_TARGET) # Závisí od finálneho produktu

# Predvolený cieľ: Zostaví celý projekt (runtime knižnicu a potom GUI)
run: all
	$(GUI_TARGET)


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
	@echo ">>> Generating documentation with Doxygen..."
	doxygen doc/Doxyfile
	@echo "<<< Documentation generated in $(DOC_DIR)/html"

# Cieľ pre Pack (zostáva podobný, kopíruješ aj nové .pro súbory)
pack:
	@echo ">>> Creating submission archive: $(ARCHIVE_NAME)..."
	$(RM) $(ARCHIVE_NAME).tar $(ARCHIVE_NAME).tar.gz $(ARCHIVE_NAME).zip
	$(MKDIR) $(ARCHIVE_NAME)
	# Skopíruj potrebné adresáre a súbory
	cp -r $(SRC_DIR) $(DOC_DIR) $(TEMPLATES_DIR) $(THIRDPARTY_DIR) \
	Makefile README.txt design.pdf $(MAIN_PRO_FILE) $(EXAMPLES_DIR) $(ARCHIVE_NAME)
	$(RM) -rf $(ARCHIVE_NAME)/$(DOC_DIR)/html
	# Vytvor archív
	$(TAR) -czf $(ARCHIVE_NAME).tar.gz -C $(ARCHIVE_NAME) . --owner=0 --group=0
	(RM) $(ARCHIVE_NAME)
	@echo "<<< Archive created: $(ARCHIVE_NAME).tar.gz"

# Pomocný cieľ
FORCE:

# Phony targets
.PHONY: all gui clean doxygen pack run FORCE
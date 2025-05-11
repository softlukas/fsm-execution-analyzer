
QMAKE    ?= qmake
MAKE     ?= make
MKDIR = mkdir -p
RM = rm -rf
TAR = tar
ZIP = zip

BUILD_DIR    = build
SRC_DIR      = src

ARCHIVE_NAME = submission
DOC_DIR = doc
TEMPLATES_DIR = templates
THIRDPARTY_DIR = third_party
EXAMPLES_DIR = examples
MAIN_PRO_FILE = main_pro.pro

GUI_EXEC_NAME = AutomationCreator
GUI_TARGET = $(BUILD_DIR)/src/gui_app/$(GUI_EXEC_NAME)


all: clean $(GUI_TARGET)

run: all
	$(GUI_TARGET)

$(GUI_TARGET): $(MAIN_PRO_FILE) $(SRC_DIR)/**/*.* FORCE
	@echo ">>> Building project using main SUBDIRS project..."
	$(MKDIR) $(BUILD_DIR)
	cd $(BUILD_DIR) && $(QMAKE) $(CURDIR)/$(MAIN_PRO_FILE) -o Makefile # Generuj hlavný Makefile v build
	$(MAKE) -C $(BUILD_DIR)  # Spusti make v build (ten zavolá make pre subdirs)
	@echo "<<< Project built successfully. GUI Tool: $(GUI_TARGET)"

clean:
	@echo ">>> Cleaning build directory..."
	$(RM) $(BUILD_DIR)
	@echo "<<< Clean finished."

doxygen:
	@echo ">>> Generating documentation with Doxygen..."
	doxygen doc/Doxyfile
	@echo "<<< Documentation generated in $(DOC_DIR)/html"

pack:
	@echo ">>> Creating submission archive: $(ARCHIVE_NAME)..."
	$(RM) $(ARCHIVE_NAME).tar $(ARCHIVE_NAME).tar.gz $(ARCHIVE_NAME).zip
	$(MKDIR) $(ARCHIVE_NAME)
	# Skopíruj potrebné adresáre a súbory
	cp -r $(SRC_DIR) $(DOC_DIR) $(TEMPLATES_DIR) $(THIRDPARTY_DIR) \
	Makefile README.txt uml.pdf $(MAIN_PRO_FILE) $(EXAMPLES_DIR) $(ARCHIVE_NAME)
	$(RM) -rf $(ARCHIVE_NAME)/$(DOC_DIR)/html
	# Vytvor archív
	$(TAR) -czf $(ARCHIVE_NAME).tar.gz -C $(ARCHIVE_NAME) . --owner=0 --group=0
	$(RM) $(ARCHIVE_NAME)
	@echo "<<< Archive created: $(ARCHIVE_NAME).tar.gz"

FORCE:

.PHONY: all gui clean doxygen pack run FORCE
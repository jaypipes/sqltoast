#
# Sqltoast
#

ACLOCAL_AMFLAGS = -I m4
LDADD = libsqltoast.la
AM_DEFAULT_SOURCE_EXT = .cc

bin_PROGRAMS =
check_PROGRAMS =
noinst_PROGRAMS =
lib_LTLIBRARIES =
noinst_LTLIBRARIES =
nobase_include_HEADERS =
noinst_HEADERS =
pkgplugin_LTLIBRARIES =
CLEANFILES =
EXTRA_DIST = \
  docs/Doxyfile \
  docs/header.html.in \
  test_run.sh

#
# libsqltoast
#

lib_LTLIBRARIES += libsqltoast.la

libsqltoast_la_SOURCES = \
	sqltoast/ast_node.cc \
	sqltoast/nodes/column_definition.cc \
	sqltoast/nodes/statements/create_table.cc \
	sqltoast/parser/create_schema.cc \
  sqltoast/parse.cc

nobase_include_HEADERS += \
  sqltoast/ast.h \
	sqltoast/ast_node.h \
	sqltoast/nodes/literal.h \
	sqltoast/nodes/identifier.h \
	sqltoast/nodes/column_default_value.h \
	sqltoast/nodes/column_definition.h \
	sqltoast/nodes/function.h \
	sqltoast/nodes/statement.h \
	sqltoast/nodes/statements/create_schema.h \
	sqltoast/nodes/statements/create_table.h \
	sqltoast/nodes/statements/drop_schema.h \
	sqltoast/nodes/statements/select.h \
  sqltoast/parser/column_definition.h \
  sqltoast/parser/common_rules.h \
  sqltoast/parser/create_schema.h \
  sqltoast/parser/create_table.h \
  sqltoast/parser/drop_schema.h \
  sqltoast/parser/identifier_attributes.h \
  sqltoast/parser/identifier_rules.h \
  sqltoast/parser/key_value_list.h \
  sqltoast/parser/specifier_attributes.h \
  sqltoast/parser/specifier_rules.h \
  sqltoast/parser/table_constraint.h \
  sqltoast/common.h \
  sqltoast/parse.h \
  sqltoast/visibility.h


libsqltoast_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
	-Wno-effc++ \
	-DBUILDING_SQLTOAST

libsqltoast_la_LIBADD = $(LIBDL_LIBS)
libsqltoast_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBSQLTOAST_VERSION)

if HAVE_LIBGTEST
check_PROGRAMS += \
	tests/table_constraints \
	tests/test_manual_ast_construction \
	tests/column_definitions \
	tests/identifiers \
	tests/test_literals

tests_test_manual_ast_construction_LDADD = libsqltoast.la ${LTLIBGTEST}
tests_column_definitions_LDADD = libsqltoast.la ${LTLIBGTEST}
tests_column_definitions_CXXFLAGS = -Wno-effc++ -std=c++0x
tests_identifiers_LDADD = libsqltoast.la ${LTLIBGTEST}
tests_identifiers_CXXFLAGS = -Wno-effc++ -std=c++0x
tests_test_literals_LDADD = libsqltoast.la ${LTLIBGTEST}
tests_table_constraints_LDADD = libsqltoast.la ${LTLIBGTEST}
tests_table_constraints_CXXFLAGS = -Wno-effc++ -std=c++0x
endif


#
# Simple sqltoast application
#

bin_PROGRAMS += bin/example
bin_example_SOURCES = bin/example.cc
bin_example_LDADD = libsqltoast.la

include config/pandora-plugin.am

TESTS = $(check_PROGRAMS)

TESTS_ENVIRONMENT = ${top_srcdir}/test_run.sh

check-verbose:
	@SQLTOAST_TEST_ARGS="v" make check

check-debug:
	@SQLTOAST_TEST_ARGS="vvv" make check

check-valgrind:
	@SQLTOAST_TEST_PREFIX="valgrind -q --leak-check=full --show-reachable=yes --suppressions=valgrind.suppressions" make check

check-valgrind-debug:
	@SQLTOAST_TEST_PREFIX="valgrind -q --leak-check=full --show-reachable=yes --suppressions=valgrind.suppressions" SQLTOAST_TEST_ARGS="vvv" make check

check-gdb:
	@SQLTOAST_TEST_PREFIX="gdb -q" make check

check-time:
	@SQLTOAST_TEST_PREFIX="/usr/bin/time" make check

check-strace:
	@SQLTOAST_TEST_PREFIX="strace -c" make check

check-truss:
	@SQLTOAST_TEST_PREFIX="truss -c" make check

docs: all
	@echo "/**" > docs/mainpage.h
	@echo "@mainpage README" >> docs/mainpage.h
	@echo "<pre>" >> docs/mainpage.h
	@cat ${top_srcdir}/README >> docs/mainpage.h
	@echo "</pre>" >> docs/mainpage.h
	@echo "*/" >> docs/mainpage.h
	${DOXYGEN} ${top_srcdir}/docs/Doxyfile
	@rm docs/mainpage.h

lcov: lcov-clean check
	@echo
	@echo "------------------------------------------------------"
	@echo "Make sure ./configure was run with '--enable-coverage'"
	@echo "------------------------------------------------------"
	@echo
	cd sqltoast && lcov --directory . --base-directory .. --capture --output-file lcov.out
	cd sqltoast && lcov --directory . --base-directory .. --extract lcov.out `pwd`/\* --output-file lcov_extract.out
	genhtml -o lcov -t sqltoast sqltoast/lcov_extract.out

lcov-clean: clean
	find . -name '*.gcno' -exec rm {} \;
	find . -name '*.gcda' -exec rm {} \;
	find . -name 'lcov*.out' -exec rm {} \;

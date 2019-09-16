FORMS           = UI/biblioteq_adminsetup.ui \
		  UI/biblioteq_allinfo.ui \
                  UI/biblioteq_bookcopybrowser.ui \
                  UI/biblioteq_bookinfo.ui \
                  UI/biblioteq_borrowers.ui \
                  UI/biblioteq_branch_s.ui \
                  UI/biblioteq_cdinfo.ui \
                  UI/biblioteq_copybrowser.ui \
                  UI/biblioteq_customquery.ui \
                  UI/biblioteq_dbenumerations.ui \
                  UI/biblioteq_dvdinfo.ui \
                  UI/biblioteq_errordiag.ui \
                  UI/biblioteq_generalmessagediag.ui \
		  UI/biblioteq_greyliteratureinfo.ui \
                  UI/biblioteq_history.ui \
                  UI/biblioteq_maginfo.ui \
                  UI/biblioteq_mainwindow.ui \
                  UI/biblioteq_members_browser.ui \
                  UI/biblioteq_otheroptions.ui \
                  UI/biblioteq_pdfreader.ui \
                  UI/biblioteq_password.ui \
                  UI/biblioteq_passwordPrompt.ui \
                  UI/biblioteq_photograph.ui \
                  UI/biblioteq_photographinfo.ui \
                  UI/biblioteq_photographview.ui \
                  UI/biblioteq_sruResults.ui \
                  UI/biblioteq_tracks.ui \
                  UI/biblioteq_userinfo.ui \
                  UI/biblioteq_videogameinfo.ui \
                  UI/biblioteq_z3950results.ui

HEADERS		= Source/biblioteq.h \
                  Source/biblioteq_bgraphicsscene.h \
		  Source/biblioteq_book.h \
		  Source/biblioteq_borrowers_editor.h \
		  Source/biblioteq_cd.h \
		  Source/biblioteq_copy_editor.h \
		  Source/biblioteq_copy_editor_book.h \
                  Source/biblioteq_dbenumerations.h \
		  Source/biblioteq_dvd.h \
		  Source/biblioteq_filesize_table_item.h \
                  Source/biblioteq_generic_thread.h \
                  Source/biblioteq_grey_literature.h \
		  Source/biblioteq_hyperlinked_text_edit.h \
		  Source/biblioteq_image_drop_site.h \
		  Source/biblioteq_item.h \
		  Source/biblioteq_magazine.h \
		  Source/biblioteq_main_table.h \
                  Source/biblioteq_myqstring.h \
                  Source/biblioteq_otheroptions.h \
                  Source/biblioteq_pdfreader.h \
		  Source/biblioteq_photographcollection.h \
                  Source/biblioteq_photograph_view.h \
		  Source/biblioteq_sruResults.h \
		  Source/biblioteq_videogame.h \
		  Source/biblioteq_z3950results.h

SOURCES		= Source/biblioteq_a.cc \
                  Source/biblioteq_b.cc \
                  Source/biblioteq_bgraphicsscene.cc \
                  Source/biblioteq_book.cc \
                  Source/biblioteq_borrowers_editor.cc \
		  Source/biblioteq_c.cc \
                  Source/biblioteq_callnum_table_item.cc \
                  Source/biblioteq_cd.cc \
                  Source/biblioteq_copy_editor.cc \
                  Source/biblioteq_copy_editor_book.cc \
                  Source/biblioteq_dbenumerations.cc \
                  Source/biblioteq_dvd.cc \
		  Source/biblioteq_filesize_table_item.cc \
                  Source/biblioteq_generic_thread.cc \
                  Source/biblioteq_grey_literature.cc \
                  Source/biblioteq_hyperlinked_text_edit.cc \
                  Source/biblioteq_image_drop_site.cc \
                  Source/biblioteq_item.cc \
                  Source/biblioteq_journal.cc \
                  Source/biblioteq_magazine.cc \
                  Source/biblioteq_main_table.cc \
                  Source/biblioteq_marc.cc \
                  Source/biblioteq_misc_functions.cc \
                  Source/biblioteq_myqstring.cc \
                  Source/biblioteq_numeric_table_item.cc \
                  Source/biblioteq_otheroptions.cc \
                  Source/biblioteq_pdfreader.cc \
                  Source/biblioteq_photographcollection.cc \
                  Source/biblioteq_photograph_view.cc \
                  Source/biblioteq_sruResults.cc \
                  Source/biblioteq_videogame.cc \
                  Source/biblioteq_z3950results.cc

TRANSLATIONS    = Translations/biblioteq_ar_JO.ts \
		  Translations/biblioteq_cs_CZ.ts \
		  Translations/biblioteq_de_DE.ts \
		  Translations/biblioteq_el_GR.ts \
		  Translations/biblioteq_fr_FR.ts \
		  Translations/biblioteq_hu_HU.ts \
		  Translations/biblioteq_it_IT.ts \
                  Translations/biblioteq_nl_BE.ts \
                  Translations/biblioteq_nl_NL.ts \
		  Translations/biblioteq_pt_PT.ts \
		  Translations/biblioteq_ru_RU.ts \
                  Translations/biblioteq_zh_CN.ts

UI_DIR          = temp

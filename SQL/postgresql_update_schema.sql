/* Version 2023.05.20 */

ALTER TABLE book ADD volume_number TEXT;
ALTER TABLE book_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE grey_literature_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE journal_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE magazine_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE photograph ALTER image SET COMPRESSION DEFAULT;
ALTER TABLE photograph_collection ALTER image SET COMPRESSION DEFAULT;

/* Version 2023.08.30 */

ALTER TABLE book ADD date_of_reform VARCHAR(32);
ALTER TABLE book ADD origin TEXT;
ALTER TABLE book ADD purchase_date VARCHAR(32);

/* Version 2024.03.25 */

ALTER TABLE book_copy_info ADD notes TEXT;
ALTER TABLE cd_copy_info ADD notes TEXT;
ALTER TABLE dvd_copy_info ADD notes TEXT;
ALTER TABLE journal_copy_info ADD notes TEXT;
ALTER TABLE magazine_copy_info ADD notes TEXT;
ALTER TABLE videogame_copy_info ADD notes TEXT;

/* Version 2024.06.30 */

DROP FUNCTION delete_request CASCADE;

/* Version 2024.10.30 */

GRANT DELETE, SELECT ON item_borrower TO biblioteq_librarian;

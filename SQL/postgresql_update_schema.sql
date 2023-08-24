/*
** Please do not load this file in its entirety.
*/

/* Release 2023.05.20 */

ALTER TABLE book ADD volume_number TEXT;

/* PostgreSQL 14 or newer is required. */

ALTER TABLE book_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE grey_literature_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE journal_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE magazine_files ALTER file SET COMPRESSION DEFAULT;
ALTER TABLE photograph ALTER image SET COMPRESSION DEFAULT;
ALTER TABLE photograph_collection ALTER image SET COMPRESSION DEFAULT;

/* Release 2023.08.30 */

ALTER TABLE book ADD date_of_reform VARCHAR(32);
ALTER TABLE book ADD origin TEXT;
ALTER TABLE book ADD purchase_date VARCHAR(32);

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

/* Version 2024.12.01 */

GRANT DELETE, SELECT ON item_borrower TO biblioteq_librarian;

/* Version 2025.06.30 */

ALTER TABLE book ADD series_title TEXT;

/* Version 2026.01.25 */

ALTER TABLE member ADD maximum_reserved_cds INTEGER NOT NULL DEFAULT 0;
ALTER TABLE member ADD maximum_reserved_dvds INTEGER NOT NULL DEFAULT 0;
ALTER TABLE member ADD maximum_reserved_grey_literatures INTEGER NOT NULL
      	    DEFAULT 0;
ALTER TABLE member ADD maximum_reserved_journals INTEGER NOT NULL DEFAULT 0;
ALTER TABLE member ADD maximum_reserved_magazines INTEGER NOT NULL DEFAULT 0;
ALTER TABLE member ADD maximum_reserved_video_games INTEGER NOT NULL
      	    DEFAULT 0;
CREATE TABLE book_statistics
(
    author		TEXT NOT NULL,
    id			VARCHAR(32),
    isbn13		VARCHAR(32),
    keyword		TEXT,
    location		TEXT NOT NULL,
    originality		TEXT,
    reserved_date	TEXT NOT NULL,
    target_audience	TEXT,
    title		TEXT NOT NULL
);
GRANT DELETE, INSERT, SELECT, UPDATE ON book_statistics TO
      biblioteq_administrator;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_statistics TO
      biblioteq_circulation;
GRANT DELETE, INSERT, SELECT, UPDATE ON book_statistics TO
      biblioteq_librarian;

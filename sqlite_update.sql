/* Release 4.04 */

ALTER TABLE book ADD offsystem_url TEXT;
ALTER TABLE cd ADD offsystem_url TEXT;
ALTER TABLE dvd ADD offsystem_url TEXT;
ALTER TABLE magazine ADD offsystem_url TEXT;
ALTER TABLE videogame ADD offsystem_url TEXT;

/* Release 5.00 */
/* Please recreate the database. */

/* Release 5.01 */

DROP VIEW cd_borrower_vw;
DROP VIEW dvd_borrower_vw;
DROP VIEW book_borrower_vw;
DROP VIEW magazine_borrower_vw;
DROP VIEW videogame_borrower_vw;

CREATE VIEW cd_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 cd_borrower;

CREATE VIEW dvd_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 dvd_borrower;

CREATE VIEW book_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 book_borrower;

CREATE VIEW magazine_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 magazine_borrower;

CREATE VIEW videogame_borrower_vw AS
SELECT	 item_oid,
	 myoid,
	 copyid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 videogame_borrower;

/* Release 5.04 */

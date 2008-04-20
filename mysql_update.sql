/* Release 3.22 */

ALTER TABLE book ADD type VARCHAR(16) NOT NULL DEFAULT 'Book';
ALTER TABLE cd ADD type VARCHAR(16) NOT NULL DEFAULT 'CD';
ALTER TABLE dvd ADD type VARCHAR(16) NOT NULL DEFAULT 'DVD';
ALTER TABLE videogame ADD type VARCHAR(16) NOT NULL DEFAULT 'Video Game';

/* Releases 3.23, 3.23.1 */

CREATE TABLE member_history
(
	memberid	 VARCHAR(16) NOT NULL,
	item_oid	 INTEGER NOT NULL,
	copyid		 VARCHAR(64) NOT NULL,
	reserved_date	 VARCHAR(32) NOT NULL,
	duedate		 VARCHAR(32) NOT NULL,
	returned_date	 VARCHAR(32) NOT NULL,
	myoid		 INTEGER PRIMARY KEY AUTO_INCREMENT,
	reserved_by	 VARCHAR(128) NOT NULL,
	type		 VARCHAR(16) NOT NULL,
	item_id		 VARCHAR(32) NOT NULL,
	FOREIGN KEY(memberid) REFERENCES member(memberid) ON DELETE CASCADE
) engine = InnoDB;

GRANT DELETE, INSERT, SELECT, UPDATE ON xbook_db.* TO xbook_admin@localhost IDENTIFIED BY 'xbook_admin';

/* Releases 4.00, 4.00.1 */

CREATE VIEW cd_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 cd_borrower;

CREATE VIEW dvd_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 dvd_borrower;

CREATE VIEW book_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 book_borrower;

CREATE VIEW magazine_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 magazine_borrower;

CREATE VIEW videogame_borrower_vw AS
SELECT	 item_oid,
	 copy_number,
	 reserved_date,
	 duedate
FROM	 videogame_borrower;

DROP USER xbook_admin@localhost;
CREATE USER xbook_admin@'%' IDENTIFIED BY 'xbook_admin';
GRANT DELETE, SELECT, UPDATE ON xbook_db.* TO xbook_admin@'%' IDENTIFIED BY 'xbook_admin';
DROP USER xbook@localhost;

/* Release 4.01 */

ALTER TABLE book DROP front_cover;
ALTER TABLE book DROP back_cover;
ALTER TABLE book ADD front_cover VARBINARY(1024);
ALTER TABLE book ADD back_cover VARBINARY(1024);
ALTER TABLE book ADD front_cover_fmt VARCHAR(8);
ALTER TABLE book ADD back_cover_fmt VARCHAR(8);
ALTER TABLE cd DROP front_cover;
ALTER TABLE cd DROP back_cover;
ALTER TABLE cd ADD front_cover VARBINARY(1024);
ALTER TABLE cd ADD back_cover VARBINARY(1024);
ALTER TABLE cd ADD front_cover_fmt VARCHAR(8);
ALTER TABLE cd ADD back_cover_fmt VARCHAR(8);
ALTER TABLE dvd DROP front_cover;
ALTER TABLE dvd DROP back_cover;
ALTER TABLE dvd ADD front_cover VARBINARY(1024);
ALTER TABLE dvd ADD back_cover VARBINARY(1024);
ALTER TABLE dvd ADD front_cover_fmt VARCHAR(8);
ALTER TABLE dvd ADD back_cover_fmt VARCHAR(8);
ALTER TABLE magazine DROP front_cover;
ALTER TABLE magazine DROP back_cover;
ALTER TABLE magazine ADD front_cover VARBINARY(1024);
ALTER TABLE magazine ADD back_cover VARBINARY(1024);
ALTER TABLE magazine ADD front_cover_fmt VARCHAR(8);
ALTER TABLE magazine ADD back_cover_fmt VARCHAR(8);
ALTER TABLE videogame DROP front_cover;
ALTER TABLE videogame DROP back_cover;
ALTER TABLE videogame ADD front_cover VARBINARY(1024);
ALTER TABLE videogame ADD back_cover VARBINARY(1024);
ALTER TABLE videogame ADD front_cover_fmt VARCHAR(8);
ALTER TABLE videogame ADD back_cover_fmt VARCHAR(8);
GRANT DELETE, SELECT, UPDATE ON xbook_db.* TO xbook_admin@'%' IDENTIFIED BY 'xbook_admin';

/* Release 4.04 */

ALTER TABLE book ADD offsystem_url LONGTEXT;
ALTER TABLE cd ADD offsystem_url LONGTEXT;
ALTER TABLE dvd ADD offsystem_url LONGTEXT;
ALTER TABLE magazine ADD offsystem_url LONGTEXT;
ALTER TABLE videogame ADD offsystem_url LONGTEXT;
GRANT DELETE, SELECT, UPDATE ON xbook_db.* TO xbook_admin@'%' IDENTIFIED BY 'xbook_admin';

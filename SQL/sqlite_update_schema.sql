/*
** Version 2023.05.20
*/

ALTER TABLE book ADD volume_number TEXT;

CREATE TRIGGER item_borrower_trigger AFTER DELETE ON member 
FOR EACH row
BEGIN
DELETE FROM item_borrower WHERE memberid = old.memberid;
END;

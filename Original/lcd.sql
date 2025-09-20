SELECT * FROM Transactions;

SELECT * FROM Favorites;

SELECT * FROM Catégories;

SELECT * FROM Master;

SELECT * FROM Transactions WHERE (Catégorie = "Épicerie" OR Catégorie = "Sorties / Cadeaux") AND Date LIKE "%-06-%";

SELECT _rowid_, * FROM Catégories;

SELECT * FROM Transactions WHERE Compte LIKE "%nationale%" AND Type != "Crédit";

UPDATE Transactions SET Description = "Remise en argent MasterCard" WHERE _rowid_ = 10;

SELECT _rowid_, * FROM Transactions WHERE Description LIKE "%:%";

SELECT _rowid_, * FROM Transactions WHERE type = "Crédit" and compte like "%nationale%";

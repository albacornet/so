DROP DATABASE IF EXISTS joc;
CREATE DATABASE joc;
USE joc;

CREATE TABLE jugadores(
id INT PRIMARY KEY NOT NULL,
pass VARCHAR(20) NOT NULL,
usuari VARCHAR(20) NOT NULL,
) ENGINE InnoDB;

CREATE TABLE partidas(
idP INT PRIMARY KEY NOT NULL,
duracio FLOAT NOT NULL,
ganador VARCHAR(20) NOT NULL,
) ENGINE InnoDB;

CREATE TABLE participaciones(
idJ INT NOT NULL,
id_P INT NOT NULL,
puntos INT NOT NULL,
FOREIGN KEY (idJ) REFERENCES jugadores (id),
FOREIGN KEY (id_P) REFERENCES jugadores (idP),
) ENGINE InnoDB;

INSERT INTO jugadores VALUES (1, 2342, 'usuari_1');

INSERT INTO jugadores VALUES (2, 5678, 'usuari_2');

INSERT INTO jugadores VALUES (3, 4563, 'usuari_3');

INSERT INTO partidas VALUES (1,11, 'usuari_1');
INSERT INTO partidas VALUES (2,15, 'usuari_2');

INSERT INTO participaciones VALUES (1,1,15);




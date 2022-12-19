DROP TABLE IF EXISTS quiz;
CREATE TABLE quiz (
	id integer primary key,
	question_text varchar(255),
	answer_1 varchar(127),
	answer_2 varchar(127),
	answer_3 varchar(127),
	answer_4 varchar(127),
	correct_answer varchar(3));

INSERT INTO quiz VALUES (1,'In ce an s-a nascut Alexandra Irina Maruta, cunoscuta drept Andra?','1984','2001','1986',"1896",'3');
INSERT INTO quiz VALUES (2,'Vocea Andrei nu se aude doar la radio! Alege filmul in care Andra dubleaza in limba romana un personaj!','Strumfii (2011)','In căutarea lui Nemo (2003)','Ted (2012) ',"The Room (2003)",'1');
INSERT INTO quiz VALUES (3,'Care este numele sotului Andrei?','Catalin Maruta','Emil Constantinescu','Nedelcu Andrew Eduard',"Stefan Avram Cherescu",'1');
INSERT INTO quiz VALUES (4,'In ce an a concertat Andra pe Stadionul "Emil Alexandrescu" din Iasi, in cadrul festivalului INIMO?','2022','2002','2021',"2020",'1');
INSERT INTO quiz VALUES (5,'Completati versurile: "Cafeneaua-n care ne-ntalneam / Acum o ocolesc / Mi-aminteste de..."','Primul sarut','Prima intalnire','Intaiul sarut','Intaiul "te iubesc"','4');

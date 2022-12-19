DROP TABLE IF EXISTS quiz;
CREATE TABLE quiz (
	id integer primary key,
	question_text varchar(255),
	answer_1 varchar(127),
	answer_2 varchar(127),
	answer_3 varchar(127),
	answer_4 varchar(127),
	correct_answer varchar(3));

INSERT INTO quiz VALUES (1,'xx?','a','b','c?',">>>>?d?<<<<",'A2');

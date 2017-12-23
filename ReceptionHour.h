#ifndef RECEPTIONHOUR_H_
#define RECEPTIONHOUR_H_

#include <unordered_map>
#include <pthread.h>

enum StudentStatus {
	ENTERED = 0,
	LEFT_BECAUSE_NO_SEAT,
	LEFT_BECAUSE_DOOR_CLOSED
};

class ReceptionHour {
public:
	ReceptionHour(unsigned int max_waiting_students);
	~ReceptionHour();

	void startStudent(unsigned int id);
	StudentStatus finishStudent(unsigned int id);

	void closeTheDoor();

	bool waitForStudent();
	void waitForQuestion();
	void giveAnswer();

	StudentStatus waitForTeacher();
	void askQuestion();
	void waitForAnswer();

protected:
	pthread_t teacher;
	pthread_t* students;

	pthread_mutex_t global_mutex;
	pthread_mutex_t counter_mutex;

	pthread_cond_t teacher_cond;
	pthread_cond_t student_cond;
	pthread_cond_t question_cond;
	pthread_cond_t answer_cond;

	StudentStatus* studentsStatus;
	unsigned int max_students;
	unsigned int students_in_room;
	bool doorClosed;
	bool askingQuestion;
	bool answeringQuestion;
	bool haveQuestion;
};

#endif // RECEPTIONHOUR_H_


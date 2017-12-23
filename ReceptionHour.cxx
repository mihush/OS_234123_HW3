#include "ReceptionHour.h"

ReceptionHour::ReceptionHour(unsigned int max_waiting_students) {
	max_students = max_waiting_students;
	students_in_room = 0;
	doorClosed = false;
	pthread_t* students = new pthread_t[max_waiting_students];

	pthread_mutex_init(&global_mutex, NULL);
	pthread_mutex_init(&counter_mutex, NULL);

	pthread_cond_init(&teacher_cond, NULL);
	pthread_cond_init(&student_cond, NULL);
	pthread_cond_init(&question_cond, NULL);
	pthread_cond_init(&answer_cond, NULL);

	studentsStatus = new StudentStatus[max_waiting_students];
	askingQuestion = false;
	haveQuestion = false;
//	for(int i = 0 ; i < max_waiting_students ; i++){TODO - check if need to initialize with start value
//
//	}
	 pthread_create(&teacher, nullptr, teacherFunc, nullptr);
}

ReceptionHour::~ReceptionHour() {
	delete[] studentsStatus;
	delete[] studentds;

	pthread_cond_destroy(&teacher_cond);
	pthread_cond_destroy(&student_cond);
	pthread_cond_destroy(&question_cond);
	pthread_cond_destroy(&answer_cond);

	pthread_mutex_destroy(&global_mutex);
	pthread_mutex_destroy(&counter_mutex);
}

void ReceptionHour::startStudent(unsigned int id) {
	pthread_create(&students[id], nullptr, studentFunc, (void*)id);

}

StudentStatus ReceptionHour::finishStudent(unsigned int id) {
	pthread_join(&students[id], nullptr);
	return 	studentsStatus[i] ;//TODO - find how to do it  ! ! !
}

void teacherFunc() {
	bool tmp = true;
	while (tmp){
		pthread_mutex_lock(&global_mutex);
		tmp = doorClosed;
		if(!tmp) { //TODO - find how to protect the "doorClosed" var
			pthread_mutex_unlock(&global_mutex);
			if (waitForStudent()) {
				waitForQuestion();
				giveAnswer();
			}
		}else{
			break;
		}
	}
	pthread_mutex_unlock(&global_mutex);
	// return and call destractor
}

void studentFunc(void* id ) {

	long i = (long)id;
	//TODO - find how to do it  ! ! !
	studentsStatus[i] = waitForTeacher();
	if(studentsStatus[i] == ENTERED){
		pthread_mutex_lock(&global_mutex);
		students_in_room++;
		pthread_mutex_unlock(&global_mutex);
		askQuestion();
		waitForAnswer();
		pthread_mutex_lock(&global_mutex);
		--students_in_room;
		pthread_mutex_unlock(&global_mutex);
	}else {
		return;
	}
	// return and call destractor
}

////////////////////////////////////////////////////
// Teacher functions
////////////////////////////////////////////////////
void ReceptionHour::closeTheDoor() {
	pthread_mutex_lock(&global_mutex);
	doorClosed = true;
	pthread_mutex_unlock(&global_mutex);
}

bool ReceptionHour::waitForStudent() {
	pthread_mutex_lock(&global_mutex);

	if (students_in_room == 0 && doorClosed) {
		pthread_mutex_unlock(&global_mutex);
		return false;
	}

	while (students_in_room == 0) {
		pthread_cond_wait(&student_cond, &global_mutex);
	}
	pthread_mutex_unlock(&global_mutex);
	return true;
}


void ReceptionHour::waitForQuestion() {
	pthread_mutex_lock(&global_mutex);
	while (askingQuestion == false) {
		pthread_cond_wait(&question_cond, &global_mutex);
	}
	haveQuestion = true;
	pthread_cond_signal(&teacher_cond);
	pthread_mutex_unlock(&global_mutex);
}


void ReceptionHour::giveAnswer() {
	pthread_mutex_lock(&global_mutex);
	while (haveQuestion == false) {
		pthread_cond_wait(&teacher_cond, &global_mutex);
	}
	answeringQuestion = true;
	pthread_cond_signal(&answer_cond);
	pthread_mutex_unlock(&global_mutex);
}

////////////////////////////////////////////////////
// Student functions
////////////////////////////////////////////////////
StudentStatus ReceptionHour::waitForTeacher() {
	pthread_mutex_lock(&global_mutex);
	if (doorClosed) {
		pthread_mutex_unlock(&global_mutex);
		return StudentStatus::LEFT_BECAUSE_DOOR_CLOSED;
	} else if (students_in_room == max_students) {
		pthread_mutex_unlock(&global_mutex);
		return StudentStatus::LEFT_BECAUSE_NO_SEAT;
	}
	pthread_mutex_unlock(&global_mutex);
	return StudentStatus::ENTERED;
}


void ReceptionHour::askQuestion() {

	pthread_mutex_lock(&global_mutex);
	while (askingQuestion == true) {
		pthread_cond_wait(&student_cond, &global_mutex);
	}
	askingQuestion = true;
	pthread_cond_signal(&question_cond);
	pthread_mutex_unlock(&global_mutex);
}

void ReceptionHour::waitForAnswer() {

	pthread_mutex_lock(&global_mutex);
	while (answeringQuestion == false) {
		pthread_cond_wait(&answer_cond, &global_mutex);
	}
	answeringQuestion = false;
	askingQuestion = false;
	haveQuestion = false;
	pthread_cond_signal(&student_cond);
	pthread_mutex_unlock(&global_mutex);
}


#ifndef _QA_DOCTOR_BASE_H_
#define _QA_DOCTOR_BASE_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct condition {
	string field;
	string value;
} Condition;

typedef vector<Condition> ConditionVec;
typedef std::string Target;

typedef enum cond_relation {
	RelationAnd 	= 10001,
	RelationOR 		= 10002
} ConditionRelation;

typedef struct cond_container {
	ConditionVec container;
	ConditionRelation relation;
} ConditionContainer;

class QuestionDoctorBase
{
public:
	virtual int MatchQuery(string query, 
		ConditionContainer &c_container, Target &target) = 0;
};

#endif

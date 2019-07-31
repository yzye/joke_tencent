#ifndef _QA_BUSI_HANDLER_H_
#define _QA_BUSI_HANDLER_H_

#include <iostream>
#include <stdlib.h>
#include <string>

#include "qa_doctor_base.h"
#include "life_service/proto/dqa_rootmerge.pb.h"

using namespace std;
using namespace pb::dqa_rootmerge;

typedef ::google::protobuf::Message GMessage;
typedef vector< GMessage *> MessageVec;

class BusiHandlerBase
{
public:
	BusiHandlerBase() : m_ans_type(DQA_UNKNOW) {};
	BusiHandlerBase(DqaAnswerType ans_type) : m_ans_type(ans_type){};
	~BusiHandlerBase() {};

	void SetQuestionDoctor(QuestionDoctorBase *qdb)
	{
		m_question_doctor = qdb;
	}

	int UnderstandQuestion(string query, 
				ConditionContainer &c_container, Target &target)
	{
		if (!m_question_doctor)
			return 0;
		return m_question_doctor->MatchQuery(query, 
				c_container, target);
	}

	virtual int PreHandle(string &query) = 0;
	// 处理请求函数
	// query :查询串
	// mv    :结果集，单个结果为一个具体业务的response protobuf
	virtual int HandleRequest(ConditionContainer c_container, 
					Target target, MessageVec &mv) = 0;
	
	virtual float GetWeight(GMessage *msg) = 0;

	virtual string GetAnsText(GMessage *msg, 
					Target target) = 0;

	virtual string SerializeToString(GMessage *msg) = 0;

	virtual void ReleaseMessage(MessageVec &mv) = 0;

	DqaAnswerType GetAnsType()
	{
		return m_ans_type;
	}

	void SetDefaultArea(string area)
	{
		m_default_area = area;
	}
private:
	DqaAnswerType m_ans_type;
	QuestionDoctorBase *m_question_doctor;
protected:
	string m_default_area;
};

#endif

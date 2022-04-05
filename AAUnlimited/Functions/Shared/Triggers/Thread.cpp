#include "StdAfx.h"




namespace Shared {
namespace Triggers {


Thread::GlobalStorage Thread::globalStorage;


void Thread::ExecuteTrigger(Trigger* trg) {
	if (trg == NULL) return;
	if (!trg->IsInitalized()) trg->Initialize(NULL,-1);
	if (trg->IsBroken()) return;
	executeCount = 0;
	maxExecuteCount = INT_MAX;
	execTrigger = trg;
	execStarted = true;
	execFinished = false;
	thisCard = trg->owningCard;

	//initialize variables
	localStorage.vars.resize(trg->vars.size());
	for(int i = 0; i < trg->vars.size(); i++) {
		localStorage.vars[i].variable = &trg->vars[i];
		localStorage.vars[i].currValue = EvaluateExpression(localStorage.vars[i].variable->defaultValue);
		if(localStorage.vars[i].currValue.type == TYPE_INVALID) {
			return;
		}
	}

	//execute actions
	CharInstData* triggeringCardInstance = &AAPlay::g_characters[this->eventData->card];
	CharInstData* thisCardInstance = &AAPlay::g_characters[execTrigger->owningCard];
	//triggers_log[idxLog] = "Trigger [" <<  execTrigger->name << "]" << "\tThis card: [" << std::to_string(thisCardInstance->m_char->m_seat) <<  "]" << "\tTriggering card: [" << std::to_string(triggeringCardInstance->m_char->m_seat) << "]\r\n";

	for(ip = 0; ip < trg->actions.size() && !execFinished; ip++) {
		//get action
		auto& action = trg->actions[ip];

		auto thisCardSeat = thisCardInstance->IsValid() ? thisCardInstance->m_char->m_seat : -1;
		auto trigCardSeat = triggeringCardInstance->IsValid() ? triggeringCardInstance->m_char->m_seat : -1;

		Shared::Triggers::triggers_log[Shared::Triggers::triggers_idxLog] = "Trigger [" + General::CastToString(execTrigger->name) + "]" + "\tThis card: [" + std::to_string(thisCardSeat) + "]" + "\tTriggering card: [" + std::to_string(trigCardSeat) + "]"+ "\tAction: " + General::CastToString(action.action->name) +"\r\n";
		Shared::Triggers::triggers_idxLog = (Shared::Triggers::triggers_idxLog + 1) % 100;
		ExecuteAction(action);
		if(executeCount > maxExecuteCount) {
			return;
		}
	}
}

bool Thread::ExecuteAction(ParameterisedAction& action) {
	if(action.action->id == ACTION_SETVAR) {
		//handle these specially
		int varId = action.actualParameters[0].varId;
		if(varId & GLOBAL_VAR_FLAG) {
			Value val = EvaluateExpression(action.actualParameters[1]);
			(*this->execTrigger->globalVars)[varId & ~GLOBAL_VAR_FLAG].currentValue = val;
		}
		else {
			VariableInstance* var = &localStorage.vars[varId];
			Value val = EvaluateExpression(action.actualParameters[1]);
			if (val.type == TYPE_INVALID) {
				return false;
			}
			var->currValue = val;
		}	
	}
	else {
		//evaluate parameters
		std::vector<Value> values;
		values.resize(action.actualParameters.size());
		for (int j = 0; j < action.actualParameters.size(); j++) {
			auto& param = action.actualParameters[j];
			values[j] = EvaluateExpression(param);
			if (values[j].type == TYPE_INVALID) {
				return false;
			}
		}
		//execute function
		(this->*(action.action->func)) (values);
	}
	
	return true;
}

Value Thread::EvaluateExpression(ParameterisedExpression& expr) {
	executeCount++;
	if(expr.expression->id == EXPR_CONSTANT) {
		//constant
		return expr.constant;
	}
	else if (expr.expression->id == EXPR_VAR) {
		//variable
		if(expr.varId & GLOBAL_VAR_FLAG) {
			return (*this->execTrigger->globalVars)[expr.varId & ~GLOBAL_VAR_FLAG].currentValue;
		}
		else {
			VariableInstance* var = &localStorage.vars[expr.varId];
			return var->currValue;
		}
	}
	else if (expr.expression->id == EXPR_NAMEDCONSTANT) {
		return expr.namedConstant->val;
	}
	else {
		if(expr.expression->returnType == TYPE_BOOL && (expr.expression->id == 4 || expr.expression->id == 5)) {
			//short circut evaluation required
			if(expr.expression->id == 4) {
				//and
				for (int j = 0; j < expr.actualParameters.size(); j++) {
					auto& param = expr.actualParameters[j];
					Value result = EvaluateExpression(param);
					if (!result.bVal) return Value(false);
				}
				return Value(true);
			}
			else {
				//or
				bool returnValue = false;
				for (int j = 0; j < expr.actualParameters.size(); j++) {
					auto& param = expr.actualParameters[j];
					Value result = EvaluateExpression(param);
					if (result.bVal) return Value(true);
				}
				return Value(false);
			}
			
		}
		else {
			//normal function
			
			//evaluate parameters
			std::vector<Value> values;
			values.resize(expr.actualParameters.size());
			for (int j = 0; j < expr.actualParameters.size(); j++) {
				auto& param = expr.actualParameters[j];
				values[j] = EvaluateExpression(param);
				if (values[j].type == TYPE_INVALID) {
					LOGPRIO(Logger::Priority::WARN) << "Thread Execution error; failed expression in trigger " <<  execTrigger->name << "\r\n";
					return Value();
				}
			}
			//execute function
			return (this->*(expr.expression->func)) (values);
		}
	}
}

}
}

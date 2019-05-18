#include "Control.h"

// Задать основание редактируемых чисел
void Control::setNumberBase(std::uint8_t base) {
	this->base = base;
}

// Выбор команды редактора
void Control::selectEditorCommand(std::uint8_t command_num) {
	if (1 <= command_num && command_num <= 15)
		number_edit.addChr(command_num);
	else 
		switch (command_num) {
			case  0: number_edit.addZero(); break;
			case 16: number_edit.addDot(); break;
			case 17: number_edit.popChr(); break;
			case 18: number_edit.clear(); 
					 processor.clear();
					 break;
			default:;
		}

	number = number_edit.getNumber();
}

// Выбор команды опаретора
void Control::selectOperatorCommand(std::uint8_t command_num) {
	switch (command_num) {
		case 19: processor.setOperation(ProcOperation::ADD); break;
		case 20: processor.setOperation(ProcOperation::SUB); break;
		case 21: processor.setOperation(ProcOperation::MULT); break;
		case 22: processor.setOperation(ProcOperation::DIV); break;
		default:;
	}
}

// Выбор команды функций
void Control::selectFunctionCommand(std::uint8_t command_num) {
	switch (command_num) {
		case 23: processor.execFunction(ProcFunction::SQR);
				 number = removeTrailingZeros(processor.getRop().getNumber()); break;
		case 24: processor.execFunction(ProcFunction::REV);
				 number = removeTrailingZeros(processor.getRop().getNumber()); break;
		default:;
	}
}

// Выбор команды памяти
void Control::selectMemoryCommand(std::uint8_t command_num) {
	switch (command_num) {
		case 25: number.empty() ?
				 memory.addNumber(PNumber(processor.getLopRes().getNumber(), base)) :
				 memory.addNumber(PNumber(number, base)); break;
		case 26: number.empty() ?
				 memory.addNumber(-PNumber(processor.getLopRes().getNumber(), base)) :
				 memory.addNumber(-PNumber(number, base)); break;
		case 27: number = removeTrailingZeros(memory.getNumber().getNumber());
				 number_edit.setNumber(number);
				 base = memory.getNumber().getP();  break;
		case 28: memory.clear(); break;
		default:;
	}
}

// Выбор команды для управления калькулятором
void Control::selectCommand(std::uint8_t command_num) {
	selectEditorCommand(command_num);		// Команды редактора числа
	selectMemoryCommand(command_num);		// Команды памяти

	if (19 <= command_num && command_num <= 24 || command_num == 29) {
		if (!number_edit.isEmpty()) {		// Распределение операнд
			if (processor.getLopRes().getNumber().empty())
				processor.setLopRes(PNumber(number_edit.getNumber(), base));
			else
				processor.setRop(PNumber(number_edit.getNumber(), base));

			number_edit.clear();
		}

		if (19 <= command_num && command_num <= 22 || command_num == 29) {
			// Выполнение операций над двумя числам
			if (processor.getOperation() != ProcOperation::NONE) {
				processor.execOperation();
				processor.setOperation(ProcOperation::NONE);

				number = removeTrailingZeros(processor.getLopRes().getNumber());
			}

			selectOperatorCommand(command_num);		// Команды выбора операций
		}

		selectFunctionCommand(command_num);			// Команды выбора функций
	}
}

// Получить строковое представление редактируемого числа
std::string Control::getNumber() const {
	return number;
}

// Проверка числа на вещественное
bool Control::isRealNum(const std::string& number) {
	return number.find('.') != std::string::npos;
}

// Удаление незначимых нулей числа
std::string Control::removeTrailingZeros(const std::string& number) {
	std::string result(number);

	if (isRealNum(result)) {
		result.erase(result.find_last_not_of('0') + 1, std::string::npos);

		if (result[result.length() - 1] == '.')
			result.pop_back();
	}

	return result;
}

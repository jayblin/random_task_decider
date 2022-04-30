#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string_view>
#include <string>
#include <vector>
#include <random>
#include <list>
#include <chrono>

#define CLOG(x) std::cout << x << "\n"

struct Args
{
	size_t days;
	size_t non_active_days;
	std::vector<std::string> names;
	std::string_view date;
};

auto get_args(int argc, char* argv[]) -> Args
{
	auto args = Args{};

	for (auto i = 1; i < argc; i++)
	{
		const auto arg = std::string_view{argv[i]};
		
		if (arg.compare("--days") == 0)
		{
			args.days = atoi(argv[i+1]);
		}
		else if (
			arg.compare("--non_active_days") == 0
			|| arg.compare("--nad") == 0
		)
		{
			args.non_active_days = atoi(argv[i+1]);
		}
		else if (arg.compare("--names") == 0)
		{
			auto count = 0;

			for (auto j = i + 1; ; ++j)
			{
				if (count > 10 || j >= (argc))
				{
					break;
				}
	 			++count;

				const auto value = std::string_view{argv[j]};

				if (value.starts_with("--"))
				{
					break;
				}

				args.names.push_back(value.data());
			}
		}
		else if (arg.compare("--date") == 0)
		{
			args.date = argv[i+1];
		}
	}

	return args;
}

auto get_work_days_list(
	const Args& args,
	std::seed_seq& seed
) -> std::list<int>
{
	std::mt19937 engine(seed);
	auto list = std::list<int>{};

	for (auto i = 1; i <= args.days; ++i)
	{
		list.push_back(i);
	}

	for (auto i = 1; i <= args.non_active_days; ++i)
	{
		std::uniform_int_distribution<int> dist(0, list.size() - 1);
		const auto advance_by = dist(engine);

		auto it = list.begin();
		std::advance(it, advance_by);

		list.erase(it);
	}

	return list;
}

auto number_to_weekday(const size_t day) -> std::string
{
	switch (day)
	{
		case 1:
			return "Понедельник";
		case 2:
			return "Вторник";
		case 3:
			return "Среда";
		case 4:
			return "Четверг";
		case 5:
			return "Пятница";
		case 6:
			return "Суббота";
		case 7:
			return "Воскресенье";
		default:
			return "[?]";
	}
}

auto get_days_per_person(const Args& args) -> size_t
{
	return (args.days - args.non_active_days) / args.names.size();
}

auto get_people_list(const Args& args) -> std::list<int>
{
	auto result = std::list<int>{};

	const auto people_count = args.names.size();
	const auto days_per_person = get_days_per_person(args);

	for (int i = 0; i < people_count; ++i)
	{
		result.push_back(i + 1);
	}

	return result;
}

auto print_result(
	const std::vector<int>& day_person_record,
	const std::vector<std::string>& names
) -> void
{
	for (auto day = 0; day < day_person_record.size(); ++day)
	{
		const auto person_id = day_person_record[day] - 1;
		const auto week_day = number_to_weekday(day + 1);

		CLOG(week_day << " " << (person_id < 0 ? "-" : names[person_id]));
	}
}

auto generate_seed_sequence(const Args& args) -> std::vector<int>
{
	auto numbers = std::vector<int>{};

	numbers.push_back(args.days);
	numbers.push_back(args.non_active_days);
	
	for (auto& name : args.names)
	{
		for (auto& ch : name) {
			numbers.push_back(ch);
		}
	}

	for (auto& ch : args.date)
	{
		numbers.push_back(ch);
	}

	return numbers;
}

int main(int argc, char* argv[])
{
	const auto args = get_args(argc, argv);

	const auto seed = generate_seed_sequence(args);

	std::seed_seq seed_seq(seed.begin(), seed.end());
	std::mt19937 engine(seed_seq);

	const auto work_days_list = get_work_days_list(args, seed_seq);
	
	auto people_list = get_people_list(args);

	auto day_person_vec = std::vector<int>{};

	for (auto i = 0; i < args.days; ++i)
	{
		day_person_vec.push_back(0);
	}

	for (auto work_day_id : work_days_list)
	{
		std::uniform_int_distribution<int> dist(
			0,
			people_list.size() - 1
		);
		const auto rnd_person_idx = dist(engine);

		auto it = people_list.begin();

		std::advance(it, rnd_person_idx);

		day_person_vec[work_day_id - 1] = *it;

		const auto active_count = std::count_if(
			day_person_vec.begin(),
			day_person_vec.end(),
			[&it](int person_id) { return person_id ==  *it;}
		);
		
		if (active_count >= get_days_per_person(args))
		{
			people_list.erase(it);
		}
	}

	print_result(day_person_vec, args.names);

	return 0;
}


#ifndef THIRDPARTY_MAP_HPP
#define THIRDPARTY_MAP_HPP

#include <utility>

template <typename CONT, typename FUNC>
class mapresult {
	CONT const &in;
	FUNC f;

   public:
	template <typename RESULT>
	RESULT to() const {
		RESULT out;
		for(auto const &e: in) {
			out.push_back(f(e));
		}
		return out;
	}
	template <typename RESULT>
	operator RESULT() const {
		return this->to<RESULT>();
	}
	mapresult(CONT const &in, FUNC f) : in(in), f(std::move(f)) {}
};

template <typename CONT, typename FUNC>
auto map(CONT const &in, FUNC f) -> mapresult<CONT, FUNC> {
	return mapresult<CONT, FUNC>(in, f);
}

#endif
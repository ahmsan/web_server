/*
 * uncopyable.h
 *
 *  Created on: 2016/06/26
 *      Author: mingsanwang
 */

#ifndef UNCOPYABLE_H_
#define UNCOPYABLE_H_


namespace ws {


class UnCopyable {
protected:
	UnCopyable() { };
	~UnCopyable() { };

private:
	UnCopyable(const UnCopyable &);
	UnCopyable& operator=(const UnCopyable &);
};

}


#endif /* UNCOPYABLE_H_ */

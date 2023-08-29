/**
 * @file ql-structure.cpp
 * @author Oliver Ofenloch (57812959+ofenloch@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2023-08-26
 *
 * @copyright Copyright (c) 2023
 *
 * Playing around and getting to know the library's structure ...
 *
 * See https://thalesians.com/aiovg_videos/luigi-ballabio-a-short-introduction-to-quantlib/
 *
 *
 * Observer / Observable example and explanation see http://cogitolearning.co.uk/2012/12/quantlib-observer-and-observable/
 * 
 * 
 * 
 * http://cogitolearning.co.uk/category/quantlib/
 * 
 * 
 * https://leanpub.com/implementingquantlib/read
 * 
 */

#include <ql/quantlib.hpp>

// Most QL objects are Obesvers or Observables (or sometimes both).
// QuantLib::Observable is an abstract base class for Observables.
// For this little program we derive our own Observable class from it:
class CMyObservable : public QuantLib::Observable {
  private:
    QuantLib::Real amount{0};
    QuantLib::Natural year{0};

  public:
    void calculateInterest(QuantLib::Real amount, QuantLib::Real rate, QuantLib::Natural years) {
        this->amount = amount;
        this->year = 0;
        notifyObservers();
        for (this->year = 1; this->year <= years; ++this->year) {
            this->amount = this->amount * (1.0 + rate);
            notifyObservers();
        }
    }
    QuantLib::Real getAmount() { return this->amount; }
}; // class CMyObservable

typedef boost::shared_ptr<CMyObservable> pMyObservable;


// Most QL objects are Obesvers or Observables (or sometimes both).
// QuantLib::Observer is an abstract base class for Observers.
// For this little program we derive our own Observer class from it:
class CMyObserver : public QuantLib::Observer {
  private:
    pMyObservable observable{nullptr};
    std::string name{""};

  public:
    CMyObserver(){};
    CMyObserver(pMyObservable obs, std::string n) : observable(obs), name(n) {
        this->registerWith(observable);
    }
    CMyObserver(const CMyObserver& observer)
    : Observer(observer), observable(observer.observable), name("Copy of " + observer.name) {}
    void update() { std::cout << name << ": " << observable->getAmount() << std::endl; }
}; // class CMyObserver


int main(int /*argv*/, char* /*argv*/[]) {

    // create an Observable
    pMyObservable calc(new CMyObservable);

    // create an Observer and connect it with the above Observable
    CMyObserver myObserverA(calc, "myObserverA");

    // create another Observable
    const QuantLib::ext::shared_ptr<CMyObservable> myObservable;

    // connect the Observer with the second Observable
    myObserverA.registerWith(myObservable);

    // create a second Observer and connect it to the calc Observable
    CMyObserver myObserverB(calc, "myObserverB");

    std::cout << "First calculation" << std::endl;
    calc->calculateInterest(100.0, 0.03, 5);

    myObserverB.unregisterWith(calc);

    std::cout << "Second calculation" << std::endl;
    calc->calculateInterest(100.0, 0.04, 5);

    return 0;
}
package elevator;

import com.oocourse.elevator3.PersonRequest;

class PersonRqWithEle {
    private PersonRequest personRequest;
    private Integer ele;

    PersonRqWithEle(PersonRequest personRequest,
                     Integer ele) {
        this.personRequest = personRequest;
        this.ele = ele;
    }

    PersonRequest GetRequest() {
        return this.personRequest;
    }

    Integer GetEle() {
        return this.ele;
    }
}

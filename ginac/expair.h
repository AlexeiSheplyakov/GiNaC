/** @file expair.h */

/** A pair of expressions.
 *  This similar to, but slightly extended STL's pair<> but we need to account
 *  for methods like .compare() */
class expair
{
public:
    expair() {}
    ~expair() {}
    expair(expair const & other) : rest(other.rest), coeff(other.coeff)
    {
        ASSERT(is_ex_exactly_of_type(coeff,numeric));
    }
    expair const & operator=(expair const & other)
    {
        if (this != &other) {
            rest=other.rest;
            coeff=other.coeff;
        }
        return *this;
    }
    expair(ex const & r, ex const & c) : rest(r), coeff(c)
    {
        ASSERT(is_ex_exactly_of_type(coeff,numeric));
    }
    
    bool is_numeric_with_coeff_1(void) const
    {
        ASSERT(is_ex_exactly_of_type(coeff,numeric));
        return is_ex_exactly_of_type(rest,numeric) &&
               (ex_to_numeric(coeff).compare(numONE())==0);
    }

    bool is_equal(expair const & other) const
    {
        return (rest.is_equal(other.rest) && coeff.is_equal(other.coeff));
    }
    bool is_less(expair const & other) const 
    {
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) && (coeff.compare(other.coeff)<0));
    }
    int compare(expair const & other) const
    {
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        cmpval=coeff.compare(other.coeff);
        return cmpval;
    }

    bool is_less_old2(expair const & other) const 
    {
        /*
        bool this_numeric_with_coeff_1=is_numeric_with_coeff_1();
        bool other_numeric_with_coeff_1=other.is_numeric_with_coeff_1();
        if (this_numeric_with_coeff_1) {
            if (other_numeric_with_coeff_1) {
                // both have coeff 1: compare rests
                return rest.compare(other.rest)<0;
            }
            // only this has coeff 1: >
            return false;
        } else if (other_numeric_with_coeff_1) {
            // only other has coeff 1: <
            return true;
        }
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) &&
                 (coeff.compare(other.coeff)<0));
        */
        if (is_ex_exactly_of_type(rest,numeric) &&
            is_ex_exactly_of_type(other.rest,numeric)) {
            if (ex_to_numeric(coeff).compare(numONE())==0) {
                if (ex_to_numeric(other.coeff).compare(numONE())==0) {
                    // both have coeff 1: compare rests
                    return rest.compare(other.rest)<0;
                }
                // only this has coeff 1: >
                return false;
            } else if (ex_to_numeric(other.coeff).compare(numONE())==0) {
                // only other has coeff 1: <
                return true;
            }
            // neither has coeff 1: usual compare        
        }
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) &&
                 (coeff.compare(other.coeff)<0));
    }
    int compare_old2(expair const & other) const
    {
        if (is_ex_exactly_of_type(rest,numeric) &&
            is_ex_exactly_of_type(other.rest,numeric)) {
            if (ex_to_numeric(coeff).compare(numONE())==0) {
                if (ex_to_numeric(other.coeff).compare(numONE())==0) {
                    // both have coeff 1: compare rests
                    return rest.compare(other.rest);
                }
                // only this has coeff 1: >
                return 1;
            } else if (ex_to_numeric(other.coeff).compare(numONE())==0) {
                // only other has coeff 1: <
                return -1;
            }
            // neither has coeff 1: usual compare        
        }
        /*
        bool this_numeric_with_coeff_1=is_numeric_with_coeff_1();
        bool other_numeric_with_coeff_1=other.is_numeric_with_coeff_1();
        if (this_numeric_with_coeff_1) {
            if (other_numeric_with_coeff_1) {
                // both have coeff 1: compare rests
                return rest.compare(other.rest);
            }
            // only this has coeff 1: >
            return 1;
        } else if (other_numeric_with_coeff_1) {
            // only other has coeff 1: <
            return -1;
            // neither has coeff 1: usual compare        
        }
        */
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        return coeff.compare(other.coeff);
    }
    bool is_less_old(expair const & other) const 
    {
        return (rest.compare(other.rest)<0) ||
               (!(other.rest.compare(rest)<0) && (coeff.compare(other.coeff)<0));
    }
    int compare_old(expair const & other) const
    {
        int cmpval=rest.compare(other.rest);
        if (cmpval!=0) return cmpval;
        cmpval=coeff.compare(other.coeff);
        return cmpval;
    }

    void printraw(ostream & os) const
    {
        os << "expair(";
        rest.printraw(os);
        os << ",";
        coeff.printraw(os);
        os << ")";
    }

    ex rest;
    ex coeff;
};

class expair_is_less
{
public:
    bool operator()(expair const & lh, expair const & rh) const
    {
        return lh.is_less(rh);
    }
};

class expair_is_less_old
{
public:
    bool operator()(expair const & lh, expair const & rh) const
    {
        return lh.is_less_old(rh);
    }
};


           

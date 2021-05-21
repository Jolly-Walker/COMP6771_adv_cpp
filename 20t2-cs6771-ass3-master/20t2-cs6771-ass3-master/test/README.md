The testing is separated into 8 test_cases
* Basic tests and Accessors
    * This case will test the default constructor, basic modifiers such as insert_node, insert_edge and clear, all the accessors and the also the ==operator
    * This is because the functions, operators and constructors tested here will be used for testing the other functionality of the graph
* Constructors
    * Here we test all the constructors. Most of the tests are straightforward, but I would like to highlight the copy constructor testing,
    where it is checked that the copy constructor creates a deep copy by modifying the copy and checking that the original was not modified
* Assignment
    * Here we test the copy assignment and move assignment. They are tested similarly to their constructor counterparts.
* Modifiers
    * The modifiers that were not tested in basic tests are tested here
    * testing of replace_node involved checking if it properly replaces the src of of edge, dst of an edge and both
    * testing of merge_replace_node does the same as replace node and also check if a duplicate edge after merge is removed
    * testing of erase_node involved getting both a true return and a false return, and checking if is_node is now false
    * testing of erase_edge involved getting both a true return and a false return, and checking if the edge is removed
* Extractor
    * here we only test the <<operator, testing for empty graph and a filled graph with multiple nodes, some nodes having multiple outgoing edges
    and some nodes having no edges
* Iterator
    * The begin, end and iterator operators are all tested here
    * simple test cases for all the sections
* find and erase with iterator
    * The find, erase(iterator i) erase(iterator i, iterator s) member functions are tested here, because the iterators need to be tested first to be able to test these functions
    * all the functions are tested using basic test cases, where return values and behaviour are checked
* Exceptions
    * Here we run all member functions that throw exceptions when certain conditions are not met
    * Checking the type of exception thrown and message body for each function


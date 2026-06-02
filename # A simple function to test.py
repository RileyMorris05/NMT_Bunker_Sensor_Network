# changee comment
def add(a, b):
    return a + b

# The test case function
def test_add_success():
    assert add(2, 3) == 5

def test_add_failure():
    assert add(2, 2) != 5

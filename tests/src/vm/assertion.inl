TEST_CASE("assertion")
{
    vmExec("assert true");
    vmThrows<AssertionError>("assert false");
}

#include <c357/base/util/percent_coding.hpp>
#include <gtest/gtest.h>

using namespace std;
using namespace c357::base::util;

TEST(persent_coding, encodes_name)
{
	string decoded = "Відсоткове_кодування";
	string expect = "%D0%92%D1%96%D0%B4%D1%81%D0%BE%D1%82%D0%BA%D0%BE%D0%B2%D0%B5_%D0%BA%D0%BE%D0%B4%D1%83%D0%B2%D0%B0%D0%BD%D0%BD%D1%8F";
	string encoded = percent_encode(decoded);
	EXPECT_EQ(expect, encoded);
}

TEST(persent_coding, encodes_url)
{
	string decoded = "https://uk.wikipedia.org/wiki/Відсоткове_кодування";
	string expect = "https://uk.wikipedia.org/wiki/%D0%92%D1%96%D0%B4%D1%81%D0%BE%D1%82%D0%BA%D0%BE%D0%B2%D0%B5_%D0%BA%D0%BE%D0%B4%D1%83%D0%B2%D0%B0%D0%BD%D0%BD%D1%8F";
	string encoded = percent_encode(decoded, "/:@!$&'()*+,;=");
	EXPECT_EQ(expect, encoded);
}

TEST(persent_coding, decodes_url)
{
	string encoded = "https://uk.wikipedia.org/wiki/%D0%92%D1%96%D0%B4%D1%81%D0%BE%D1%82%D0%BA%D0%BE%D0%B2%D0%B5_%D0%BA%D0%BE%D0%B4%D1%83%D0%B2%D0%B0%D0%BD%D0%BD%D1%8F";
	string expect = "https://uk.wikipedia.org/wiki/Відсоткове_кодування";
	string decoded = percent_decode(encoded);
	EXPECT_EQ(expect, decoded);
}

#include "gtest/gtest.h"
#include "config_parser.h"

#include <sstream>
#include <string>

// Class to quickly test sample configuration files
class NginxFileConfigTest : public ::testing::Test {
protected:
	NginxConfigParser parser;
    NginxConfig out_config;
    bool ParseFile(const char* file_name) {
		return parser.Parse(file_name, &out_config);
	}
};

// Tests given example configuration file
TEST_F(NginxFileConfigTest, ExampleConfig) {
	EXPECT_TRUE(ParseFile("example_config"));
}

TEST_F(NginxFileConfigTest, MimeTypes) {
	EXPECT_TRUE(ParseFile("mime_types"));
}

TEST_F(NginxFileConfigTest, NginxWebsiteConfig) {
	EXPECT_TRUE(ParseFile("nginx_website_config"));
}

// Class to quickly test literal strings 
class NginxStringConfigTest : public ::testing::Test {
protected:
	NginxConfigParser parser_;
	NginxConfig config_;
	bool ParseString(const std::string& config_string) {
		std::stringstream config_stream(config_string);
		return parser_.Parse(&config_stream, &config_);
	}
};

TEST_F(NginxStringConfigTest, SimpleConfig) {
	EXPECT_TRUE(ParseString("foo bar;"));
	EXPECT_EQ(1, config_.statements_.size());
	EXPECT_EQ("foo", config_.statements_.at(0)->tokens_.at(0));
}

TEST_F(NginxStringConfigTest, EmptyConfig) {
	EXPECT_FALSE(ParseString(""));
}

TEST_F(NginxStringConfigTest, MissingSemicolon) {
	EXPECT_FALSE(ParseString("foo bar"));
}

TEST_F(NginxStringConfigTest, DoubleNestedConfig) {
	EXPECT_TRUE(ParseString("foo { bar { baz aux; } }"));
}

TEST_F(NginxStringConfigTest, ExtraEndBlockSymbol) {
	EXPECT_FALSE(ParseString("foo { bar baz; } }"));
}

TEST_F(NginxStringConfigTest, ExtraStartBlockSymbol) {
	EXPECT_FALSE(ParseString("foo { { bar baz; }"));
}

TEST_F(NginxStringConfigTest, CurlyBraceHasSpecialMeaning) {
	EXPECT_FALSE(ParseString("a };"));
	EXPECT_FALSE(ParseString("{ a;"));
}

TEST_F(NginxStringConfigTest, SemicolonInName) {
	EXPECT_TRUE(ParseString("a \";\";"));
	EXPECT_TRUE(ParseString("\";\" a;"));
}

TEST_F(NginxStringConfigTest, SquareBrackets) {
	EXPECT_TRUE(ParseString("[ ];"));
}

TEST_F(NginxStringConfigTest, CurlyBracesInQuotes) {
	EXPECT_TRUE(ParseString("\"{}\" 2;"));
}

TEST_F(NginxStringConfigTest, SpecialCharactersTest1) {
	EXPECT_TRUE(ParseString("| { @ { ^ *; } }"));
}

TEST_F(NginxStringConfigTest, SpecialCharactersTest2) {
	EXPECT_TRUE(ParseString("/ { \"/\" { + :; } }"));
}

TEST_F(NginxStringConfigTest, EmptyBodyTest) {
	EXPECT_FALSE(ParseString("a {}"));
}

TEST_F(NginxStringConfigTest, UnmatchedQuoteTest) {
	EXPECT_FALSE(ParseString("\" a;"));
}

TEST_F(NginxStringConfigTest, ThreeQuoteTest) {
	EXPECT_FALSE(ParseString("\"\"\" a;"));
}

TEST_F(NginxStringConfigTest, EmptyString) {
	EXPECT_TRUE(ParseString("\"\" a;"));
}

TEST_F(NginxStringConfigTest, CommentTest) {
	EXPECT_TRUE(ParseString("a b; # A comment"));
}

TEST_F(NginxStringConfigTest, HashtagMeaning) {
	EXPECT_FALSE(ParseString("a #;"));
}

TEST_F(NginxStringConfigTest, BodyWithoutName) {
	EXPECT_FALSE(ParseString(" { a b; } "));
}

TEST_F(NginxStringConfigTest, NoNewLine) {
	EXPECT_TRUE(ParseString("a b; c d; e {f g;} h k {l m;}"));
}

TEST_F(NginxStringConfigTest, Wade1) {
	EXPECT_FALSE(ParseString("hello;}"));
}


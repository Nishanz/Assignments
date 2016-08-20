import re
import string

"""
Attributions:

The regexes are mostly attributed. Reason being these regexes are pretty robust and developing a better regex than these is hard. An exception being making my own regex for the Unicode encoding for words with Apostrophes or dashes.

Regexes for Emoticons, Phone numbers, Twitter Usernames & hashtags and parts of Crazy English words is sourced from Christopher Potts's work on Tokenization. Link being: http://sentiment.christopherpotts.net/code-data/happyfuntokenizing.py
Regexes for Website Links is sourced from Stackoverflow. Link being: http://stackoverflow.com/questions/3809401/what-is-a-good-regular-expression-to-match-a-url

Also, the concept of not lowering the characters if the character is an emoticon is attributed to Christopher Potts. The reason of inclusion being: Word variation is greatly reduced by lower casing the string, but if the emoticons cannot be lower-cased otherwise they cannot be printed.
"""

exclude = set(string.punctuation)

emoticons = r"""
    (?:
      [<>]?
      [:;=8]                     # eyes
      [\-o\*\']?                 # optional nose
      [\)\]\(\[dDpP/\:\}\{@\|\\] # mouth      
      |
      [\)\]\(\[dDpP/\:\}\{@\|\\] # mouth
      [\-o\*\']?                 # optional nose
      [:;=8]                     # eyes
      [<>]?
    )"""

phone_numbers = r"""
    (?:
      (?:            # (international)
        \+?[01]
        [\-\s.]*
      )?            
      (?:            # (area code)
        [\(]?
        \d{3}
        [\-\s.\)]*
      )?    
      \d{3}          # exchange
      [\-\s.]*   
      \d{4}          # base
    )"""

website_links = r"""(?:https?:\/\/(?:www\.|(?!www))[^\s\.]+\.[^\s]{2,}|www\.[^\s]+\.[^\s]{2,})"""
twitter_usernames = r"""(?:@[\w_]+)"""
twitter_hashtags = r"""(?:\#+[\w_]+[\w\'_\-]*[\w_]+)"""

crazy_english_words = r"""
    (?:[a-z][a-z'\-\xC2-\xDF\xE0\xA0-\xBF\xED\x80-\x9F\xE1-\xEC\xEE-\xEF\xF0\x90-\xBF\xF4\x80-\x8F\xF1-\xF3\x80-\xBF\x80-\xBF\x80-\xBF_]+[a-z])       # Words with apostrophes or dashes.
    |
    (?:[+\-]?\d+[,/.:-]\d+[+\-]?)  # Numbers, including fractions, decimals.
    |
    (?:[\w_]+)                     # Words without apostrophes or dashes.
    |
    (?:\.(?:\s*\.){1,})            # Ellipsis dots. 
    |
    (?:\S)                         # Everything else that isn't whitespace.
"""   
# The components of the tokenizer:
regex_strings = (emoticons,phone_numbers,website_links,twitter_hashtags,twitter_usernames,crazy_english_words)

word_re = re.compile(r"""(%s)""" % "|".join(regex_strings), re.VERBOSE | re.I | re.UNICODE)
emoticon_re = re.compile(emoticons, re.VERBOSE | re.I | re.UNICODE)

f = open('tweets_en.txt','r')
lines = f.read().decode('utf8')
f.close()
lines =lines.split('\n')[:-1]

tokenized_lines = []

for line in lines:
  
  words = word_re.findall(line)
  #words = [x for x in words if x not in exclude]

  words = map((lambda x : x if emoticon_re.search(x) else x.lower()), words)
  words = map((lambda x : x.encode('utf8')),words)
  tokenized_lines.append(words)

f = open('tweets_tokenized.txt','w')
for line in tokenized_lines:
  if line != []:
    print(line)
    for word in line:
      f.write(word)
      f.write(' ')
    f.write('\n')
f.close()
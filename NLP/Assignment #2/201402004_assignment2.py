import matplotlib.pyplot as plt
import re
import string
import math
#"""
#Attributions:
#
#The regexes are mostly attributed. Reason being these regexes are pretty robust and developing a better regex than these is hard. An exception being making my own regex for the Unicode encoding for words with Apostrophes or dashes.
#
#Regexes for Emoticons, Phone numbers, Twitter Usernames & hashtags and parts of Crazy English words is sourced from Christopher Potts's work on Tokenization. Link being: http://sentiment.christopherpotts.net/code-data/happyfuntokenizing.py
#Regexes for Website Links is sourced from Stackoverflow. Link being: http://stackoverflow.com/questions/3809401/what-is-a-good-regular-expression-to-match-a-url
#
#Also, the concept of not lowering the characters if the character is an emoticon is attributed to Christopher Potts. The reason of inclusion being: Word variation is greatly reduced by lower casing the string, but if the emoticons cannot be lower-cased otherwise they cannot be printed.
#"""
#
#exclude = set(string.punctuation)
#
#emoticons = r"""
#    (?:
#      [<>]?
#      [:;=8]                     # eyes
#      [\-o\*\']?                 # optional nose
#      [\)\]\(\[dDpP/\:\}\{@\|\\] # mouth      
#      |
#      [\)\]\(\[dDpP/\:\}\{@\|\\] # mouth
#      [\-o\*\']?                 # optional nose
#      [:;=8]                     # eyes
#      [<>]?
#    )"""
#
#phone_numbers = r"""
#    (?:
#      (?:            # (international)
#        \+?[01]
#        [\-\s.]*
#      )?            
#      (?:            # (area code)
#        [\(]?
#        \d{3}
#        [\-\s.\)]*
#      )?    
#      \d{3}          # exchange
#      [\-\s.]*   
#      \d{4}          # base
#    )"""
#
#website_links = r"""(?:https?:\/\/(?:www\.|(?!www))[^\s\.]+\.[^\s]{2,}|www\.[^\s]+\.[^\s]{2,})"""
#twitter_usernames = r"""(?:@[\w_]+)"""
#twitter_hashtags = r"""(?:\#+[\w_]+[\w\'_\-]*[\w_]+)"""
#
#crazy_english_words = r"""
#    (?:[a-z][a-z'\-\xC2-\xDF\xE0\xA0-\xBF\xED\x80-\x9F\xE1-\xEC\xEE-\xEF\xF0\x90-\xBF\xF4\x80-\x8F\xF1-\xF3\x80-\xBF\x80-\xBF\x80-\xBF_]+[a-z])       # Words with apostrophes or dashes.
#    |
#    (?:[+\-]?\d+[,/.:-]\d+[+\-]?)  # Numbers, including fractions, decimals.
#    |
#    (?:[\w_]+)                     # Words without apostrophes or dashes.
#    |
#    (?:\.(?:\s*\.){1,})            # Ellipsis dots. 
#    |
#    (?:\S)                         # Everything else that isn't whitespace.
#"""   
## The components of the tokenizer:
#regex_strings = (emoticons,phone_numbers,website_links,twitter_hashtags,twitter_usernames,crazy_english_words)
#
#sentence_re = re.compile(r'[^.!?\s][^.!?]*(?:[.!?](?![\'"]?\s|$)[^.!?]*)*[.!?]?[\'"]?(?=\s|$)', re.MULTILINE | re.DOTALL)
#
#
#word_re = re.compile(r"""(%s)""" % "|".join(regex_strings), re.VERBOSE | re.I | re.UNICODE)
#emoticon_re = re.compile(emoticons, re.VERBOSE | re.I | re.UNICODE)
#
#f = open('sherlock.txt','r')
#lines = f.read()
#f.close()
#lines = sentence_re.findall(lines)
##print(lines)
#tokenized_lines = []
#
#for line in lines:
#  words = word_re.findall(line)
#  words = [x for x in words if x not in exclude]
#  
#  tokenized_lines.append(words)
#
#f = open('sherlock_tokenized.txt','w')
#for line in tokenized_lines:
#  if line != []:
#    #print(line)
#    f.write('<s> ')
#    for word in line:
#      f.write(word)
#      f.write(' ')
#    f.write('<\s>')
#    f.write('\n')
#f.close()

f = open('sherlock_tokenized.txt','r')
lines = f.read().lower()
f.close()
lines = lines.split('\n')

unigramcount = {}
bigramcount = {}
trigramcount = {}
fourgramcount = {}
fivegramcount = {}
sixgramcount = {}
onegram ={}
twogram = {}
threegram = {}
fourgram = {}
fivegram = {}
sixgram = {}
nextwordunigram = {}
nextwordbigram = {}
nextwordtrigram = {}
nextwordfourgram = {}
nextwordfivegram = {}
nextwordsixgram = {}

for line in lines:
  #print(line)
  line = line.split(' ')
  ll = len(line)
  #print(ll)
  #print(line)
  for idx in xrange(len(line)):

    if (idx == ll-2 and idx>=0):
      if onegram.has_key(line[idx]):
        onegram[line[idx]] += 1
      else:
        onegram[line[idx]] = 1

    if (idx == ll-3 and idx>=0):
      if twogram.has_key(line[idx]+' '+line[idx+1]):
        twogram[line[idx]+' '+line[idx+1]] += 1
      else:
        twogram[line[idx]+' '+line[idx+1]] = 1

    if (idx == ll-4 and idx>=0):
      if threegram.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]):
        threegram[line[idx]+' '+line[idx+1]+' '+line[idx+2]] += 1
      else:
        threegram[line[idx]+' '+line[idx+1]+' '+line[idx+2]] = 1

    if (idx == ll-5 and idx>=0):
      if fourgram.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]):
        fourgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]] += 1
      else:
        fourgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]] = 1

    if (idx == ll-6 and idx>=0):
      if fivegram.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]):
        fivegram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]] += 1
      else:
        fivegram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]] = 1

    if (idx == ll-7 and idx>=0):
      if sixgram.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]):
        sixgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]] += 1
      else:
        sixgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]] = 1

    if(idx <= ll-2 and idx >=0):
      if unigramcount.has_key(line[idx]):
        unigramcount[line[idx]] += 1
      else:
        unigramcount[line[idx]] = 1

    if (idx <= ll-3 and idx>=0):
      if bigramcount.has_key(line[idx]+' '+line[idx+1]):
        bigramcount[line[idx]+' '+line[idx+1]] += 1
      else:
        bigramcount[line[idx]+' '+line[idx+1]] = 1

    if (idx <= ll-4 and idx>=0):
      if trigramcount.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]):
        trigramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]] += 1
      else:
        trigramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]] = 1

    if (idx <= ll-5 and idx>=0):
      if fourgramcount.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]):
        fourgramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]] += 1
      else:
        fourgramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]] = 1

    if (idx <= ll-6 and idx>=0):
      if fivegramcount.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]):
        fivegramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]] += 1
      else:
        fivegramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]] = 1

    if (idx == ll-7 and idx>=0):
      if sixgramcount.has_key(line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]):
        sixgramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]] += 1
      else:
        sixgramcount[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]] = 1

for line in lines:
  #print(line)
  line = line.split(' ')
  ll = len(line)
  #print(ll)
  #print(line)
  for idx in xrange(len(line)):
    if(idx <= ll-2 and idx >=0):
      if not nextwordunigram.has_key(line[idx]):
        nextwordunigram[line[idx]] = line[idx+1]
      try:
        if unigramcount[line[idx]] > unigramcount[nextwordunigram[line[idx]]]:
          nextwordunigram[line[idx]] = line[idx+1]
      except:
        pass

    if (idx <= ll-3 and idx>=0):
      nextwordbigram[line[idx]+' '+line[idx+1]] = line[idx+2]

    if (idx <= ll-4 and idx>=0):
        nextwordtrigram[line[idx]+' '+line[idx+1]+' '+line[idx+2]] = line[idx+3]

    if (idx <= ll-5 and idx>=0):
        nextwordfourgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]] = line[idx+4]

    if (idx <= ll-6 and idx>=0):
        nextwordfivegram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]] = line[idx+5]

    if (idx == ll-7 and idx>=0):
        nextwordsixgram[line[idx]+' '+line[idx+1]+' '+line[idx+2]+' '+line[idx+3]+' '+line[idx+4]+' '+line[idx+5]] = line[idx+6]

uniprob = 0.0
biprob = 0.0
triprob = 0.0
fourprob = 0.0
fiveprob = 0.0
sixprob = 0.0

for token in onegram:
  uniprob += (onegram[token]/unigramcount[token])

for token in twogram:
  biprob += (twogram[token]/bigramcount[token])

for token in threegram:
  triprob += (threegram[token]/trigramcount[token])

for token in fourgram:
  fourprob += (fourgram[token]/fourgramcount[token])

for token in fivegram:
  fiveprob += (fivegram[token]/fivegramcount[token])

for token in sixgram:
  sixprob += (sixgram[token]/sixgramcount[token])

try:
  uniprob /= len(onegram) 
  biprob /= len(twogram)
  triprob /= len(threegram)
  fourprob /= len(fourgram)
  fiveprob /= len(fivegram)
  sixprob /= len(sixgram)
except:
  print("Horrible error!")

print("Part A: The probabilities of tokens being sentence ending tokens are as follows:")
print("Unigram: "+str(uniprob))
print("Bigrams: "+str(biprob))
print("Trigrams: "+str(triprob))
print("Fourgrams: "+str(fourprob))
print("Fivegrams: "+str(fiveprob))
print("Sixgrams: "+str(sixprob))
print("Hence, the probability order is: Sixgrams > Fivegrams > Fourgrams > Trigrams >> Bigrams >> Unigrams")

print('Part B:')
y = list(reversed(sorted(unigramcount.values())))
x= []
logy= []
for idx in xrange(len(unigramcount)):
  x.append(idx+1)
  logy.append(math.log10(y[idx]))

f = plt.figure(0)
plt.plot(x,y)
plt.ylabel('Frequency of unigrams')
plt.xlabel('Rank')
#plt.show()

#plt.plot(x,logy)
#plt.ylabel('Log Frequency')
#plt.xlabel('Rank')
#plt.show()

print('Part C:')
i = 1
for ngram in [onegram,twogram,threegram,fourgram,fivegram,sixgram]:
  plt.figure(i)
  y = list(reversed(sorted(ngram.values())))
  x = [idx for idx in xrange(len(ngram))]
  plt.plot(x,y)
  plt.ylabel('Frequency of '+str(i)+' gram model for P(X|<\s>)')
  plt.xlabel('Rank')
  axes=plt.gca()
  axes.set_ylim([0,max(y)])
  i+=1
  #plt.show()

plt.show()

noinputs = 1
while(noinputs>=0):
  print('Part D:')
  inp = raw_input('Enter the value of \'n\' in the n-gram model to generate sentences. Enter \'-1\' to exit.\n')
  noinputs = int(inp) - 1
  idx =0
  
  line = []
  line.append('<s>')
  if noinputs>0 and noinputs<7:
    inputs = raw_input('Enter '+str(noinputs)+' words to start the sentence\n')
    inputs = inputs.strip('\n').split(' ')
    for sinput in inputs:
      line.append(sinput)

  while(noinputs>=0 and line[-1]!='<\s>'):
    try:
      if noinputs == 0:
        while line[-1]!='<\s>':
          line.append(nextwordunigram[line[-1]])

      if noinputs == 1:
        while line[-1]!='<\s>':
          line.append(nextwordbigram[line[-2]+' '+line[-1]])
  
      if noinputs == 2:
        while line[-1]!='<\s>':
          line.append(nextwordtrigram[line[-3]+' '+line[-2]+' '+line[-1]])

      if noinputs == 3:
        while line[-1]!='<\s>':
          line.append(nextwordfourgram[line[-4]+' '+line[-3]+' '+line[-2]+' '+line[-1]])
  
      if noinputs == 4:
        while line[-1]!='<\s>':
          line.append(nextwordfivegram[line[-5]+' '+line[-4]+' '+line[-3]+' '+line[-2]+' '+line[-1]])
  
      if noinputs == 5:
        while line[-1]!='<\s>':
          line.append(nextwordsixgram[line[-6]+' '+line[-5]+' '+line[-4]+' '+line[-3]+' '+line[-2]+' '+line[-1]])
    
    except:
      noinputs -= 1 
      print('Sorry! '+str(noinputs+2)+' gram not found! Using Katz Backoff to revert to '+str(noinputs+1)+' gram model!')
    #print(noinputs)
    #print(line)

  if(noinputs>=0):
    print(line)
  
  else:
    print("Sorry! Even unigrams were missing! Katz Backoff failed!")
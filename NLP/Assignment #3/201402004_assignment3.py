import matplotlib.pyplot as plt
import re
import string
import math

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

sentence_re = re.compile(r'[^.!?\s][^.!?]*(?:[.!?](?![\'"]?\s|$)[^.!?]*)*[.!?]?[\'"]?(?=\s|$)', re.MULTILINE | re.DOTALL)


word_re = re.compile(r"""(%s)""" % "|".join(regex_strings), re.VERBOSE | re.I | re.UNICODE)
emoticon_re = re.compile(emoticons, re.VERBOSE | re.I | re.UNICODE)

def fileprocess(inputFile,outputFile):
    f = open(inputFile,'r')
    lines = f.read()
    f.close()
    lines = sentence_re.findall(lines)
    #print(lines)
    tokenized_lines = []
    
    for line in lines:
      words = word_re.findall(line)
      words = [x.lower() for x in words if x not in exclude]
      
      tokenized_lines.append(words)
    
    f = open(outputFile,'w')
    for line in tokenized_lines:
      if line != [] and len(line)>3:
        #print(line)
        f.write('<s> ')
        for word in line:
          f.write(word)
          f.write(' ')
        f.write('<\s>')
        f.write('\n')
    f.close()

#fileprocess('John Galsworthy.txt','galsworthy_tokenized.txt')
#fileprocess('John Stuart Mill.txt','mill_tokenized.txt')
#fileprocess('P B Shelley.txt','shelley_tokenized.txt')
#fileprocess('William Makepeace Thackeray.txt','thackeray_tokenized.txt')
#fileprocess('William Wordsworth.txt','wordsmith_tokenized.txt')
#fileprocess('John Galsworthy Test.txt','galsworthy_tokenized_test.txt')
#fileprocess('John Stuart Mill Test Hard.txt','mill_tokenized_test.txt')
#fileprocess('P B Shelley Test Hard.txt','shelley_tokenized_test.txt')
#fileprocess('William Makepeace Thackeray Test.txt','thackeray_tokenized_test.txt')
#fileprocess('William Wordsworth Test Very Hard.txt','wordsmith_tokenized_test.txt')

alpha = 0.40 # This has been empirically found to be a good constant in the Backoff introduced in the paper.
maxval = 5
vocabulary_size = 5000

# Added features - Vocabulary restriction (deletion) and lowering
unigramcounts1 = []
unigramcounts2 = []
unigramcounts3 = []
unigramcounts4 = []
unigramcounts5 = []
bigramcounts1 = []
bigramcounts2 = []
bigramcounts3 = []
bigramcounts4 = []
bigramcounts5 = []
trigramcounts1 = []
trigramcounts2 = []
trigramcounts3 = []
trigramcounts4 = []
trigramcounts5 = []
ngrams1 = {}
ngrams2 = {}
ngrams3 = {}
ngrams4 = {}
ngrams5 = {}
estimatenextngrams1 = {}
estimatenextngrams2 = {}
estimatenextngrams3 = {}
estimatenextngrams4 = {}
estimatenextngrams5 = {}
nocorpus = 1
bigramfreq = {}
unigramfreq = {}

def score_lm(nextword,ngram,ngrams,freq,val):
	scoreval = float(0.0)

	if ngram+nextword in ngrams:
		#print(str(ngram+nextword))
		wordfreq = ngrams[ngram+nextword]
		#print(wordfreq)
		if freq < 250:
			scoreval += float((wordfreq+1))*(float(freq[wordfreq+1])/float(freq[wordfreq])) 
		
	if ngram+nextword not in ngrams and val >= 2: #Backoff
		ngram = ngram.split(' ')[:-1]
		new = ''
		for word in ngram[1:]:
			new += word+' '
		scoreval += float(alpha) * float(score_lm(nextword,new,ngrams,freq, val-1)) 
	
	elif ngram+nextword in ngrams:
		scoreval += (float(ngrams[ngram+nextword])/float(ngrams[ngram]))
		#print(ngrams)
		#((float(ngrams[ngram+nextword])/float(ngrams[ngram])))
		#print(scoreval)
	#print(scoreval)
	return scoreval



for file in ['galsworthy_tokenized.txt','mill_tokenized.txt','shelley_tokenized.txt','thackeray_tokenized.txt','wordsmith_tokenized.txt', 'galsworthy_tokenized_test.txt','mill_tokenized_test.txt','shelley_tokenized_test.txt','thackeray_tokenized_test.txt','wordsmith_tokenized_test.txt']:
    if nocorpus ==1:
        ngrams = ngrams1
        nextngrams = estimatenextngrams1
        unigramcounts = unigramcounts1
     	bigramcounts =  bigramcounts1
    	trigramcounts = trigramcounts1

    if nocorpus ==2:
        ngrams = ngrams2
        nextngrams = estimatenextngrams2
        unigramcounts = unigramcounts2
     	bigramcounts =  bigramcounts2
    	trigramcounts = trigramcounts2

    if nocorpus ==3:
        ngrams = ngrams3
        nextngrams = estimatenextngrams3
        unigramcounts = unigramcounts3
     	bigramcounts =  bigramcounts3
    	trigramcounts = trigramcounts3

    if nocorpus ==4:
        ngrams = ngrams4
        nextngrams = estimatenextngrams4
        unigramcounts = unigramcounts4
     	bigramcounts =  bigramcounts4
    	trigramcounts = trigramcounts4

    if nocorpus ==5:
        ngrams = ngrams5
        nextngrams = estimatenextngrams5
        unigramcounts = unigramcounts5
     	bigramcounts =  bigramcounts5
    	trigramcounts = trigramcounts5

    bigramlist = []
    trigramlist = []
    unigramlist = []
    fourgramlist = []
    fivegramlist = []
    ngrams = {}

    f = open(file)
    lines = f.read().split('\n')[:-1]

    wordcount={}
    for line in lines:
        for word in line.split(' '):
            if word not in wordcount:
                wordcount[word] = 1.0
            else:
                wordcount[word] += 1.0

    vocabulary = sorted(wordcount, key=wordcount.__getitem__, reverse=True)
    print(len(vocabulary))
    vocabulary = vocabulary[:vocabulary_size]
    print(len(vocabulary))

    print('Vocabulary created! Rest are <UNK>')

    vocabulary.append('<UNK>')

    for i in xrange(len(lines)):
    	lines[i] = lines[i].split(' ')
    	for j in xrange(len(lines[i])):
    		if lines[i][j] not in vocabulary:
    			lines[i][j] = '<UNK>'

    for line in lines:
    	for j in xrange(len(line)):
    		rng = min(maxval,len(line)-j)
    		ngram = ''
    		for idx in xrange(rng):
    			ngram += line[j+idx]+' '
    			if ngram not in ngrams:
    				ngrams[ngram] = 1.0
    			else:
    				ngrams[ngram] += 1.0
    			if idx == 0:
    				unigramlist.append(ngram)
    			if idx == 1:
    				bigramlist.append(ngram)
    			if idx == 2:
    				trigramlist.append(ngram)
    			if idx == 3:
    				fourgramlist.append(ngram)
    			if idx == 4:
    				fivegramlist.append(ngram)

    unigramlist = list(set(unigramlist))
    bigramlist = list(set(bigramlist))
    trigramlist = list(set(trigramlist))
    fourgramlist = list(set(fourgramlist))
    fivegramlist = list(set(fivegramlist))

    i = 0	
    for ngramlist in [unigramlist,bigramlist,trigramlist,fourgramlist,fivegramlist]:
    	count = {}
    	for word in ngramlist:
    		if ngrams[word] in count:
    			count[ngrams[word]] += 1.0
    		else:
    			count[ngrams[word]] = 1.0
    	if i == 0:
    		unigramfreq = count
    	if i == 1:
    		bigramfreq = count
    	if i == 2:
    		trigramfreq = count
    	if i == 3:
    		fourgramfreq = count
    	if i == 4:
    		fivegramfreq = count
    	i += 1
    #print(unigramfreq)
    #print(fourgramfreq)
    #print(fivegramfreq)
    print('The number of unigrams are:'+str(len(unigramlist)))
    print('The number of bigrams are:'+str(len(bigramlist)))
    print('The number of trigrams are:'+str(len(trigramlist)))
    print('The number of fourgrams are: '+str(len(fourgramlist)))
    print('The number of fivegrams are: '+str(len(fivegramlist)))

    for line in lines:
    	for j in xrange(len(line)):
    		rng = min(maxval,len(line)-j)
    		ngram = ''
    		for idx in xrange(rng-1):
    			ngram += line[j+idx]+' '
    			if idx == 0:
    				freq = unigramfreq
    			if idx == 1:
    				freq = bigramfreq 
    			if idx == 2:
    				freq = trigramfreq 
    			if idx == 3:
    				freq = fourgramfreq 
    			if idx == 4:
    				freq = fivegramfreq 

    			if ngram not in nextngrams:
    				nextngrams[ngram] = line[j+idx+1]+' '
    			elif score_lm(line[j+idx+1]+' ',ngram,ngrams,freq,idx+1) > score_lm(nextngrams[ngram],ngram,ngrams,freq,idx+1):
    				nextngrams[ngram] = line[j+idx+1]+' '

   	#print('Comparing the bigrams to differentiate...')
   	#print(nextngrams)
    for unigram in unigramlist:
    	unigramcounts.append(math.log(float(ngrams[unigram])))

    for bigram in bigramlist:
    	bigramcounts.append(math.log(float(ngrams[bigram])))

    for trigram in trigramlist:
    	trigramcounts.append(math.log(float(ngrams[trigram])))

    #minunigram = min(unigramcounts)
    #maxunigram = max(unigramcounts)
    #for idx in xrange(len(unigramcounts)):
    #	unigramcounts[idx] = (unigramcounts[idx]-minunigram)/(maxunigram-minunigram)
    #minbigram = min(bigramcounts)
    #maxbigram = max(bigramcounts)
    #for idx in xrange(len(bigramcounts)):
    #	bigramcounts[idx] = (bigramcounts[idx]-minbigram)/(maxbigram-minbigram)
    #mintrigram = min(trigramcounts)
    #maxtrigram = max(trigramcounts)
    #for idx in xrange(len(trigramcounts)):
    #	trigramcounts[idx] = (trigramcounts[idx]-mintrigram)/(maxtrigram-mintrigram)

    unigramcounts = sorted(unigramcounts, reverse=True)
    bigramcounts = sorted(bigramcounts, reverse=True)
    trigramcounts = sorted(trigramcounts, reverse=True)

    plt.figure(0)
    if nocorpus == 1:
    	plt.plot(unigramcounts[:2500],'b:',label='Glasworthy Train')
    if nocorpus == 2:
    	plt.plot(unigramcounts[:2500],'m:',label='Mill Train')
    if nocorpus == 3:
    	plt.plot(unigramcounts[:2500],'r:',label='Shelley Train')
    if nocorpus == 4:
    	plt.plot(unigramcounts[:2500],'k:',label='Thackarey Train')
    if nocorpus == 5:
    	plt.plot(unigramcounts[:2500],'c:',label='Wordsmith Train')
    if nocorpus == 6:
    	plt.plot(unigramcounts[:2500],'b--',label='Glasworthy Test')
    if nocorpus == 7:
    	plt.plot(unigramcounts[:2500],'m--',label='Mill Test')
    if nocorpus == 8:
    	plt.plot(unigramcounts[:2500],'r--',label='Shelley Test')
    if nocorpus == 9:
    	plt.plot(unigramcounts[:2500],'k--',label='Thackarey Test')
    if nocorpus == 10:
    	plt.plot(unigramcounts[:2500],'c--',label='Wordsmith Test')
    plt.xlabel('Rank of Unigrams')
    plt.ylabel('Frequency of Unigrams')
    plt.legend(loc='upper center', shadow=True)
    plt.figure(1)
    if nocorpus == 1:
    	plt.plot(bigramcounts[:2500],'b:',label='Glasworthy Train')
    if nocorpus == 2:
    	plt.plot(bigramcounts[:2500],'m:',label='Mill Train')
    if nocorpus == 3:
    	plt.plot(bigramcounts[:2500],'r:',label='Shelley Train')
    if nocorpus == 4:
    	plt.plot(bigramcounts[:2500],'k:',label='Thackarey Train')
    if nocorpus == 5:
    	plt.plot(bigramcounts[:2500],'c:',label='Wordsmith Train')
    if nocorpus == 6:
    	plt.plot(bigramcounts[:2500],'b--',label='Glasworthy Test')
    if nocorpus == 7:
    	plt.plot(bigramcounts[:2500],'m--',label='Mill Test')
    if nocorpus == 8:
    	plt.plot(bigramcounts[:2500],'r--',label='Shelley Test')
    if nocorpus == 9:
    	plt.plot(bigramcounts[:2500],'k--',label='Thackarey Test')
    if nocorpus == 10:
    	plt.plot(bigramcounts[:2500],'c--',label='Wordsmith Test')
    plt.xlabel('Rank of Bigrams')
    plt.ylabel('Frequency of Bigrams')
    plt.legend(loc='upper center', shadow=True)
    plt.figure(2)
    if nocorpus == 1:
    	plt.plot(trigramcounts[:2500],'b:',label='Glasworthy Train')
    if nocorpus == 2:
    	plt.plot(trigramcounts[:2500],'m:',label='Mill Train')
    if nocorpus == 3:
    	plt.plot(trigramcounts[:2500],'r:',label='Shelley Train')
    if nocorpus == 4:
    	plt.plot(trigramcounts[:2500],'k:',label='Thackarey Train')
    if nocorpus == 5:
    	plt.plot(trigramcounts[:2500],'c:',label='Wordsmith Train')
    if nocorpus == 6:
    	plt.plot(trigramcounts[:2500],'b--',label='Glasworthy Test')
    if nocorpus == 7:
    	plt.plot(trigramcounts[:2500],'m--',label='Mill Test')
    if nocorpus == 8:
    	plt.plot(trigramcounts[:2500],'r--',label='Shelley Test')
    if nocorpus == 9:
    	plt.plot(trigramcounts[:2500],'k--',label='Thackarey Test')
    if nocorpus == 10:
    	plt.plot(trigramcounts[:2500],'c--',label='Wordsmith Test')
    plt.xlabel('Rank of Trigrams')
    plt.ylabel('Frequency of Trigrams')
    plt.legend(loc='upper center', shadow=True)
    if nocorpus == 1:
        ngrams1 = ngrams
        estimatenextngrams1 = nextngrams
        unigramcounts1 = unigramcounts
     	bigramcounts1 =  bigramcounts
    	trigramcounts1 = trigramcounts

    if nocorpus == 2:
        ngrams2 = ngrams
        estimatenextngrams2 = nextngrams
        unigramcounts2 = unigramcounts
     	bigramcounts2 =  bigramcounts
    	trigramcounts2 = trigramcounts


    if nocorpus == 3:
        ngrams3 = ngrams
        estimatenextngrams3 = nextngrams
        unigramcounts3 = unigramcounts
     	bigramcounts3 =  bigramcounts
    	trigramcounts3 = trigramcounts


    if nocorpus == 4:
        ngrams4 = ngrams
        estimatenextngrams4 = nextngrams
        unigramcounts4 = unigramcounts
     	bigramcounts4 =  bigramcounts
    	trigramcounts4 = trigramcounts


    if nocorpus == 5:
        ngrams5 = ngrams
        estimatenextngrams5 = nextngrams
        unigramcounts5 = unigramcounts
     	bigramcounts5 =  bigramcounts
    	trigramcounts5 = trigramcounts

    nocorpus += 1

plt.show()


line = 'NLP Rocks!'

while(line != 'Quit'):
	line = raw_input('Enter a sentence for the language model!\n')
	if line == 'Quit':
		break
	try:
		line = line.strip('\n').split(' ')
	except:
		continue
	score1 = 0.0
	score2 = 0.0
	score3 = 0.0
	score4 = 0.0
	score5 = 0.0
	print line

	for idx in xrange(len(line)-1):
		sentence = line[idx]+' '
		nextword = line[idx+1]+' '
		ngrams = ngrams1
		#print(ngrams)
		score1 += score_lm(nextword,sentence,ngrams,bigramfreq, 1)
		ngrams = ngrams2
		score2 += score_lm(nextword,sentence,ngrams,bigramfreq, 1)
		ngrams = ngrams3
		score3 += score_lm(nextword,sentence,ngrams,bigramfreq, 1)
		ngrams = ngrams4
		score4 += score_lm(nextword,sentence,ngrams,bigramfreq, 1)
		ngrams = ngrams5
		score5 += score_lm(nextword,sentence,ngrams,bigramfreq, 1)

	print(score1,score2,score3,score4,score5)


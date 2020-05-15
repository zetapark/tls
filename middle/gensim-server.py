#!/usr/bin/python3
# coding: utf-8

# In[1]:


import gensim
from zeta import Server


# In[21]:


file = ['/home/zeta/Downloads/word2vec/GoogleNews-slim.bin', '/home/zeta/Downloads/word2vec/ko.bin']
kor = gensim.models.Word2Vec.load(file[1])
eng = gensim.models.keyedvectors.KeyedVectors.load_word2vec_format(file[0], binary=True)
vocab_eng = eng.vocab.keys()
vocab_kor = kor.wv.vocab.keys()


# In[40]:


def f(s):
    print(s)
    li = s.split()
    r =  ''
    for i in zip(li[::2], li[1::2]): 
        if i[0] in vocab_eng and i[1] in vocab_eng:
            r += str(eng.similarity(i[0], i[1])) + ' '
        elif i[0] in vocab_kor and i[1] in vocab_kor:
            r += str(kor.wv.similarity(i[0], i[1])) + ' '
        else: r += '0 '
    print(r)
    return r


# In[71]:


sv = Server(3003)


# In[ ]:


sv.nokeep_start(f)


import usd_notice_broker as u
from pxr import Usd

s = Usd.Stage.CreateInMemory()

def testPredicate(notice):
    print("predicate")
    print(notice)
    #print(dir(notice))
    return True

cache = u.NoticeCache(u.TestNoticeWrapper())

print(dir(u))
a = u.TestNoticeWrapper.Init(5)
myBroker = u.NoticeBroker.Create(s)
myBroker.BeginTransaction(testPredicate)
print(myBroker.IsInTransaction())
s.DefinePrim("/root")
myBroker.Process(a)
myBroker.EndTransaction()
print(myBroker.IsInTransaction())


print("INIT B")
b = u.TestNoticeWrapper.Init(2)
b.Send()
b.Send()
print("GETALL")
print(cache.GetAll())
print("MERGEALL")
print(cache.MergeAll())
print("GETALL")
print(cache.GetAll())
print(cache.GetAll()[0].GetCount())

'''
myBroker = u.NoticeBroker.Create(s)
print(dir(u))
print(dir(myBroker))
myBroker.BeginTransaction(testPredicate)
print(myBroker.IsInTransaction())
s.DefinePrim("/root")
notice = u.TestNoticeWrapper.Init(1)
print(notice)
'''
'''
print(u.TestNotice(1))
#myBroker.Process(u.TestNotice(1))
myBroker.EndTransaction()
print(myBroker.IsInTransaction())
'''

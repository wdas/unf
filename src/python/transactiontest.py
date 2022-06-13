import usd_notice_broker as u
from pxr import Usd

s = Usd.Stage.CreateInMemory()

def testPredicate(notice):
    print("predicate")
    print(notice)
    return True

print(dir(u))
a = u.TestNoticeWrapper.Init(5)
myBroker = u.NoticeBroker.Create(s)
myBroker.BeginTransaction(testPredicate)
print(myBroker.IsInTransaction())
s.DefinePrim("/root")
myBroker.Process(a)
myBroker.EndTransaction()
print(myBroker.IsInTransaction())
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

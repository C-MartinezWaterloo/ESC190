class CircularQueue:
    def __init__(self, capacity):
        self.data = [None] * capacity    
        self.capacity = capacity
        self.size = 0
        self.first = 0
        self.last = 0

    def enqueue(self, value):
        
        self.data[self.last] = value
        self.last = (self.last + 1) % self.capacity
        self.size += 1

    def dequeue(self):
        
        value = self.data[self.first]
        self.data[self.first] = None  
        self.first = (self.first + 1) % self.capacity
        self.size -= 1

        return value


    def __iter__(self):
        

        idx = self.front
        count = 0
        while count < self.size:
            yield self.data[idx]
            idx = (idx + 1) % self.capacity
            count += 1

    def __lt__(self, other):
       
        return list(self) < list(other)

    def __str__(self):
        
        return "->".join(str(x) for x in self if x is not None)

if __name__ == '__main__':
    cq = CircularQueue(5)
    cq.enqueue(10)
    cq.enqueue(20)
    cq.enqueue(30)
    print(cq.data)
    print(cq.dequeue())
    print(cq.data)

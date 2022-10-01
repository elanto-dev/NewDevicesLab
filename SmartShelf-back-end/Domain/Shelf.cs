using System.Collections.Generic;
using Domain.Identity;

namespace Domain
{
    public class Shelf
    {
        public int Id { get; set; }
        public string Identifier { get; set; }
        public bool Main { get; set; } = true;
        public int? MainShelfId { get; set; }
        public string Location { get; set; }
        public int UserId { get; set; }
        public User User { get; set; }
        public List<ShelfInTrip> ShelfInShelvesSetups { get; set; }
    }
}

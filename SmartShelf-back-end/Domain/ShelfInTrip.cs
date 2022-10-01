using System;
using System.Collections.Generic;
using System.Text;

namespace Domain
{
    public class ShelfInTrip
    {
        public int Id { get; set; }
        public int ShelfId { get; set; }
        public Shelf Shelf { get; set; }
        public int TripId { get; set; }
        public Trip Trip { get; set; }
        public List<ShelfSetupCategories> ShelfSetupCategories { get; set; }
    }
}

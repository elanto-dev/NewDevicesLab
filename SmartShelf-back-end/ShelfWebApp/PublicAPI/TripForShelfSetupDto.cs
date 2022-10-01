using System;
using System.Collections.Generic;

namespace ShelfWebApp.PublicAPI
{
    public class TripForShelfSetupDto
    {
        public int TripId { get; set; }
        // We allow to check out one hour before the trip
        public DateTime EarliestCheckOutDateTime { get; set; }
        public DateTime ReminderDateTime { get; set; }
        public List<CategoriesSetupDto> Categories { get; set; }

        public class CategoriesSetupDto
        {
            public string Name { get; set; }
            public int Index { get; set; }
        }
    }
}

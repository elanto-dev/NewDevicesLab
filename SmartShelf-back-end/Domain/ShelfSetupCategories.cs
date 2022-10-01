namespace Domain
{
    public class ShelfSetupCategories
    {
        public int Id { get; set; }
        public int Index { get; set; }
        public bool ItemRemoved { get; set; }
        public int CategoryId { get; set; }
        public Category Category { get; set; }
        public int ShelfInTripId { get; set; }
        public ShelfInTrip ShelfInTrip { get; set; }
    }
}

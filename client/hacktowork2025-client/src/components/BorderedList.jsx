function BorderedList({ children, border = false }) {
  return (
    <div>
      {children.map((child, index) => (
        <div key={index} className={index < children.length - 1 && border ? 'pt-2 pb-6 mb-4 border-b border-neutral-200' : ''}>
          {child}
        </div>
      ))}
    </div>
  );
}

export default BorderedList;